from typing import Type, Optional
from copy import deepcopy

from pydantic import BaseModel


def create_update_schema(create_schema: Type[BaseModel]) -> type:
    update_schema = type(
        f'Update{create_schema.__name__}',
        create_schema.__bases__,
        dict(create_schema.__dict__)
    )

    update_schema.__fields__ = deepcopy(update_schema.__fields__)
    for field in update_schema.__fields__.values():
        field.required = False

    return update_schema


def create_list_schema(entity_schema: Type[BaseModel]) -> type:
    class ListSchema(BaseModel):
        count: int
        entities: list[entity_schema]

    return type(
        f'List{entity_schema.__name__}',
        ListSchema.__bases__,
        dict(ListSchema.__dict__)
    )
