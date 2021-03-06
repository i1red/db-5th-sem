from typing import Optional, Type

from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from sqlalchemy import Table

import db
from .common import create_update_schema, create_list_schema


def create_one_to_one_router(main_plural_name: str, referencing_singular_name: str,
                             referencing_plural_name: str, table: Table, fk_name: str, schema_in: Type[BaseModel],
                             schema_out: Type[BaseModel], tags: Optional[list[str]] = None) -> APIRouter:
    router = APIRouter(tags=tags or [referencing_plural_name])

    update_schema = create_update_schema(schema_in)
    list_schema = create_list_schema(schema_out)

    @router.get(f'/{main_plural_name}/{{fk}}/{referencing_singular_name}/', response_model=schema_out)
    def get_referencing_entity(fk: int) -> dict:
        referencing_entity = db.get_entity(table, {fk_name: fk})
        if referencing_entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**referencing_entity).dict()

    @router.post(f'/{main_plural_name}/{{fk}}/{referencing_singular_name}/', response_model=schema_out)
    def create_referencing_entity(fk: int, referencing_entity: schema_in) -> dict:
        referencing_entity = db.create_entity(table, {fk_name: fk, **referencing_entity.dict()})
        return schema_out(**referencing_entity).dict()

    @router.patch(f'/{main_plural_name}/{{fk}}/{referencing_singular_name}/', response_model=schema_out)
    def update_referencing_entity(fk: int, referencing_entity: update_schema) -> dict:
        values = referencing_entity.dict(exclude_unset=True)
        if not values:
            raise HTTPException(status_code=422)

        referencing_entity = db.update_entity_by_unique_value(table, fk_name, fk, values)
        if referencing_entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**referencing_entity).dict()

    @router.delete(f'/{main_plural_name}/{{fk}}/{referencing_singular_name}/', response_model=schema_out)
    def delete_referencing_entity(fk: int) -> dict:
        referencing_entity = db.delete_entity(table, {fk_name: fk})
        if referencing_entity is None:
            raise HTTPException(status_code=404)

        return schema_out(**referencing_entity).dict()

    @router.get(f'/{referencing_plural_name}/', response_model=list_schema)
    def list_referencing_entities() -> dict:
        entities = db.list_entities(table)
        return list_schema(count=len(entities), entities=[schema_out(**entity).dict() for entity in entities]).dict()

    return router
