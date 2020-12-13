from typing import Any

from sqlalchemy import create_engine, Table, and_

from models import metadata

_USER = 'postgres'
_PASSWORD = 'postgres'
_HOST = 'localhost:5432'
_DBNAME = 'combat_sports_db'

_ENGINE = create_engine(f"postgresql+psycopg2://{_USER}:{_PASSWORD}@{_HOST}/{_DBNAME}")
metadata.create_all(_ENGINE)


def get_entity_by_unique_value(table: Table, column_name: str, column_value: Any) -> dict:
    with _ENGINE.connect() as connection:
        return connection.execute(
            table.select().where(getattr(table.c, column_name) == column_value)
        ).fetchone()


def create_entity(table: Table, values: dict) -> dict:
    with _ENGINE.connect() as connection:
        return connection.execute(
            table.insert().values(**values).returning(*table.c)
        ).fetchone()


def update_entity_by_unique_value(table: Table, column_name: str, column_value: Any, updated_values: dict) -> dict:
    with _ENGINE.connect() as connection:
        return connection.execute(
            table.update().values(**updated_values)
                .where(getattr(table.c, column_name) == column_value).returning(*table.c)
        ).fetchone()


def delete_entity_by_unique_value(table: Table, column_name: str, column_value: Any) -> dict:
    with _ENGINE.connect() as connection:
        return connection.execute(
            table.delete().where(getattr(table.c, column_name) == column_value).returning(*table.c)
        ).fetchone()


def delete_entity(table: Table, conditions: dict[str, Any]) -> dict:
    with _ENGINE.connect() as connection:
        return connection.execute(
            table.delete().where(and_(*[getattr(table.c, column_name) == column_value
                                        for column_name, column_value in conditions.items()])).returning(*table.c)
        ).fetchone()


def list_entities(table: Table) -> list[dict]:
    with _ENGINE.connect() as connection:
        return list(connection.execute(table.select()).fetchall())


def list_entities_by_condition(table: Table, column_name: str, column_value: Any) -> list[dict]:
    with _ENGINE.connect() as connection:
        return list(connection.execute(table.select().where(getattr(table.c, column_name) == column_value)).fetchall())


def get_entities_after_joining_tables(main_table: Table, binding_table: Table, column_name: str, column_value: Any) -> list:
    with _ENGINE.connect() as connection:
        print(main_table.join(binding_table).select(*main_table.c).where(getattr(binding_table.c, column_name) == column_value))
        return list(connection.execute(
            main_table.join(binding_table).select(*main_table.c).where(getattr(binding_table.c, column_name) == column_value)
        ).fetchall())