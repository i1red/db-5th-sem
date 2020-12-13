from typing import Optional, Type

from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from sqlalchemy import Table

import db
from .common import create_update_schema, create_list_schema


def create_one_to_many_router(main_plural_name: str, referencing_plural_name: str,
                              table: Table, fk_name: str, schema_in: Type[BaseModel],
                              schema_out: Type[BaseModel], tags: Optional[list[str]] = None) -> APIRouter:
    router = APIRouter(tags=tags or [referencing_plural_name])

    update_schema = create_update_schema(schema_in)
    list_schema = create_list_schema(schema_out)

    @router.get(f'/{main_plural_name}/{{fk}}/{referencing_plural_name}/', response_model=list_schema)
    def get_referencing_entities(fk: int) -> dict:
        referencing_entities = db.list_entities_by_condition(table, fk_name, fk)
        return list_schema(count=len(referencing_entities), entities=referencing_entities).dict()

    @router.post(f'/{main_plural_name}/{{fk}}/{referencing_plural_name}/', response_model=schema_out)
    def create_referencing_entity(fk: int, entity: schema_in) -> dict:
        entity = db.create_entity(table, {fk_name: fk, **entity.dict()})
        return schema_out(**entity).dict()

    @router.get(f'/{referencing_plural_name}/{{id}}', response_model=schema_out)
    def get_entity(id: int) -> dict:
        entity = db.get_entity_by_unique_value(table, 'id', id)
        if entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**entity).dict()

    @router.patch(f'/{referencing_plural_name}/{{id}}/', response_model=schema_out)
    def update_entity(id: int, entity: update_schema) -> dict:
        values = entity.dict(exclude_unset=True)
        if not values:
            raise HTTPException(status_code=422)

        entity = db.update_entity_by_unique_value(table, 'id', id, values)
        if entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**entity).dict()

    @router.delete(f'/{referencing_plural_name}/{{id}}/', response_model=schema_out)
    def delete_referencing_entity(id: int) -> dict:
        referencing_entity = db.delete_entity_by_unique_value(table, 'id', id)
        if referencing_entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**referencing_entity).dict()

    @router.get(f'/{referencing_plural_name}/', response_model=list_schema)
    def list_referencing_entities() -> dict:
        entities = db.list_entities(table)
        return list_schema(count=len(entities), entities=[schema_out(**entity).dict() for entity in entities]).dict()

    return router
