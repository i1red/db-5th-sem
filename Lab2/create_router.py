from typing import Optional, Type
from copy import deepcopy

from fastapi import APIRouter, HTTPException
from pydantic import BaseModel

from dbconnection import create_connection


def _create_update_schema(create_schema: Type[BaseModel]) -> type:
    update_schema = type(
        f'Update{create_schema.__name__}',
        create_schema.__bases__,
        dict(create_schema.__dict__)
    )

    update_schema.__fields__ = deepcopy(update_schema.__fields__)
    for _, field in update_schema.__fields__.items():
        field.required = False

    return update_schema


def _create_list_schema(entity_schema: Type[BaseModel]) -> type:
    class ListSchema(BaseModel):
        count: int
        entities: list[entity_schema]

    return type(
        f'List{entity_schema.__name__}',
        ListSchema.__bases__,
        dict(ListSchema.__dict__)
    )


def create_router(plural_entity_name: str, table, schema_in, schema_out, tags: Optional[list[str]] = None):
    router = APIRouter(prefix=f'/{plural_entity_name}', tags=tags or [plural_entity_name])

    @router.get('/{id}', response_model=schema_out)
    def get_entity(id: int) -> dict:
        with create_connection() as connection:
            entity = connection.execute(
                table.select().where(table.c.id == id)
            ).fetchone()

            if entity is None:
                raise HTTPException(status_code=404)

            return schema_out(**entity).dict()

    @router.post('/', response_model=schema_out)
    def create_entity(entity: schema_in) -> dict:
        with create_connection() as connection:
            entity = connection.execute(
                table.insert().values(**entity.dict()).returning(*table.c)
            ).fetchone()
            return schema_out(**entity).dict()

    @router.patch('/{id}', response_model=schema_out)
    def update_entity(id: int, entity: _create_update_schema(schema_in)) -> dict:
        with create_connection() as connection:
            entity_dict = entity.dict(exclude_unset=True)

            if len(entity_dict) == 0:
                raise HTTPException(status_code=422)

            entity = connection.execute(
                table.update().values(**entity_dict).where(table.c.id == id).returning(*table.c)
            ).fetchone()

            if entity is None:
                raise HTTPException(status_code=404)

            return schema_out(**entity).dict()

    @router.delete('/{id}', response_model=schema_out)
    def delete_entity(id: int) -> dict:
        with create_connection() as connection:
            entity = connection.execute(
                table.delete().where(table.c.id == id).returning(*table.c)
            ).fetchone()

            if entity is None:
                raise HTTPException(status_code=404)

            return schema_out(**entity).dict()

    @router.get('/', response_model=_create_list_schema(schema_out))
    def list_entity() -> dict:
        with create_connection() as connection:
            entities = connection.execute(
                table.select()
            ).fetchall()

            return {'count': len(entities), 'entities': [schema_out(**entity).dict() for entity in entities]}

    return router