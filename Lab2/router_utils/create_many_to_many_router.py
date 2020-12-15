from typing import Optional, Type

from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from sqlalchemy import Table

import db
from .common import create_list_schema


def create_many_to_many_router(first_plural_name: str, second_plural_name: str,
                               first_table: Table, second_table: Table,
                               binding_table: Table, first_fk_name: str, second_fk_name: str,
                               first_schema_in: Type[BaseModel], first_schema_out: Type[BaseModel],
                               second_schema_in: Type[BaseModel], second_schema_out: Type[BaseModel],
                               binding_schema_in: Type[BaseModel], binding_schema_out: Type[BaseModel],
                               tags: Optional[list[str]] = None) -> APIRouter:
    router = APIRouter(tags=tags or [f'{first_plural_name} & {second_plural_name}'])

    @router.post(f'/{first_plural_name}/{{first_fk}}/{second_plural_name}/{{second_fk}}/')
    def create_binding(first_fk: int, second_fk: int, binding: binding_schema_in) -> dict:
        binding_entity = db.create_entity(
            binding_table, {first_fk_name: first_fk, second_fk_name: second_fk, **binding.dict()}
        )
        return binding_schema_out(**binding_entity).dict()

    @router.delete(f'/{first_plural_name}/{{first_fk}}/{second_plural_name}/{{second_fk}}/')
    def create_binding(first_fk: int, second_fk: int, binding: binding_schema_in) -> dict:
        binding_entity = db.delete_entity(
            binding_table, {first_fk_name: first_fk, second_fk_name: second_fk, **binding.dict()}
        )
        return binding_schema_out(**binding_entity).dict()

    setups = [
        (first_plural_name, second_plural_name, second_table, second_fk_name, second_schema_in, second_schema_out),
        (second_plural_name, first_plural_name, first_table, first_fk_name, first_schema_in, first_schema_out)
    ]
    for main_name, referencing_name, table, fk_name, schema_in, schema_out in setups:
        list_schema = create_list_schema(schema_out)

        @router.get(f'/{main_name}/{{main_fk}}/{referencing_name}/', response_model=list_schema)
        def list_entities(main_fk: int) -> dict:
            entities = db.get_entities_after_joining_tables(table, binding_table, fk_name, main_fk)
            return list_schema(count=len(entities), entities=entities)

    return router
