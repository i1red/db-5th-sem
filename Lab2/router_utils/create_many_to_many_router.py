from typing import Optional, Type

from fastapi import APIRouter, HTTPException
from pydantic import BaseModel
from sqlalchemy import Table

import db
from .common import create_list_schema


def create_many_to_many_router(first_plural_name: str, second_plural_name: str,
                               first_table: Table, second_table: Table,
                               binding_table: Table, first_fk_name: str, second_fk_name: str,
                               first_schema_out: Type[BaseModel], second_schema_out: Type[BaseModel],
                               binding_schema_in: Type[BaseModel], binding_schema_out: Type[BaseModel],
                               tags: Optional[list[str]] = None) -> APIRouter:
    router = APIRouter(tags=tags or [f'{first_plural_name} & {second_plural_name}'])

    @router.get(f'/{first_plural_name}/{{first_fk}}/{second_plural_name}/{{second_fk}}/',
                response_model=binding_schema_out)
    def get_binding(first_fk: int, second_fk: int) -> dict:
        binding_entity = db.get_entity(binding_table, {first_fk_name: first_fk, second_fk_name: second_fk})
        if binding_entity is None:
            raise HTTPException(status_code=404)

        return binding_schema_out(**binding_entity).dict()

    @router.post(f'/{first_plural_name}/{{first_fk}}/{second_plural_name}/{{second_fk}}/',
                 response_model=binding_schema_out)
    def create_binding(first_fk: int, second_fk: int, binding: binding_schema_in) -> dict:
        binding_entity = db.create_entity(
            binding_table, {first_fk_name: first_fk, second_fk_name: second_fk, **binding.dict()}
        )
        return binding_schema_out(**binding_entity).dict()

    @router.delete(f'/{first_plural_name}/{{first_fk}}/{second_plural_name}/{{second_fk}}/',
                   response_model=binding_schema_out)
    def delete_binding(first_fk: int, second_fk: int) -> dict:
        binding_entity = db.delete_entity(binding_table, {first_fk_name: first_fk, second_fk_name: second_fk})
        if binding_entity is None:
            raise HTTPException(status_code=404)

        return binding_schema_out(**binding_entity).dict()

    binding_list_schema = create_list_schema(binding_schema_out)

    @router.get(f'/{first_plural_name}&{second_plural_name}/', response_model=binding_list_schema)
    def list_binding_entities() -> dict:
        binding_entities = db.list_entities(binding_table)
        return binding_list_schema(count=len(binding_entities), entities=binding_entities)

    second_list_schema = create_list_schema(second_schema_out)

    @router.get(f'/{first_plural_name}/{{main_fk}}/{second_plural_name}/', response_model=second_list_schema)
    def list_entities(main_fk: int) -> dict:
        entities = db.list_entities_after_joining_tables(second_table, binding_table, first_fk_name, main_fk)
        return second_list_schema(count=len(entities), entities=entities).dict()

    first_list_schema = create_list_schema(first_schema_out)

    @router.get(f'/{second_plural_name}/{{main_fk}}/{first_plural_name}/', response_model=first_list_schema)
    def list_entities(main_fk: int) -> dict:
        entities = db.list_entities_after_joining_tables(first_table, binding_table, second_fk_name, main_fk)
        return first_list_schema(count=len(entities), entities=entities).dict()

    return router
