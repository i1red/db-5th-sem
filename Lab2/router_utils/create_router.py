from typing import Optional, Type

from sqlalchemy import Table
from fastapi import APIRouter, HTTPException
from pydantic import BaseModel

import db
from .common import create_list_schema, create_update_schema


def create_router(plural_name: str, table: Table,
                  schema_in: Type[BaseModel], schema_out: Type[BaseModel], tags: Optional[list[str]] = None) -> APIRouter:
    router = APIRouter(prefix=f'/{plural_name}', tags=tags or [plural_name])

    update_schema = create_update_schema(schema_in)
    list_schema = create_list_schema(schema_out)

    @router.get('/{id}', response_model=schema_out)
    def get_entity(id: int) -> dict:
        entity = db.get_entity(table, {'id': id})
        if entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**entity).dict()

    @router.post('/', response_model=schema_out)
    def create_entity(entity: schema_in) -> dict:
        entity = db.create_entity(table, entity.dict())
        return schema_out(**entity).dict()

    @router.patch('/{id}', response_model=schema_out)
    def update_entity(id: int, entity: update_schema) -> dict:
        values = entity.dict(exclude_unset=True)
        if not values:
            raise HTTPException(status_code=422)

        entity = db.update_entity_by_unique_value(table, 'id', id, values)
        if entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**entity).dict()

    @router.delete('/{id}', response_model=schema_out)
    def delete_entity(id: int) -> dict:
        entity = db.delete_entity(table, {'id': id})
        if entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**entity).dict()

    @router.get('/', response_model=list_schema)
    def list_entities() -> dict:
        entities = db.list_entities(table)
        return list_schema(count=len(entities), entities=[schema_out(**entity).dict() for entity in entities]).dict()

    return router
