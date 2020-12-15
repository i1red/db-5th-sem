from typing import Type, Optional, Callable
from copy import deepcopy

from pydantic import BaseModel


def cache_function(prefix: str):
    def wrapper(func: Callable) -> Callable:
        schemas = {}

        def schema_creator(schema: Type[BaseModel]) -> type:
            schema_name = f'{prefix}{schema.__name__}'
            if schema_name not in schemas:
                schemas[schema_name] = func(schema, prefix)

            return schemas[schema_name]

        return schema_creator

    return wrapper


@cache_function('Update')
def create_update_schema(create_schema: Type[BaseModel], prefix: str) -> type:
    update_schema = type(
        f'{prefix}{create_schema.__name__}',
        create_schema.__bases__,
        dict(create_schema.__dict__)
    )

    update_schema.__fields__ = deepcopy(update_schema.__fields__)
    for field in update_schema.__fields__.values():
        field.required = False

    return update_schema


@cache_function('List')
def create_list_schema(entity_schema: Type[BaseModel], prefix: str) -> type:
    class ListSchema(BaseModel):
        count: int
        entities: list[entity_schema]

    return type(
        f'{prefix}{entity_schema.__name__}',
        ListSchema.__bases__,
        dict(ListSchema.__dict__)
    )
