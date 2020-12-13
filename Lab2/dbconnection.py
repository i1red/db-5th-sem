import sqlalchemy

from models import metadata

_USER = 'postgres'
_PASSWORD = 'postgres'
_HOST = 'localhost:5432'
_DBNAME = 'combat_sports_db'

_ENGINE = sqlalchemy.create_engine(f"postgresql+psycopg2://{_USER}:{_PASSWORD}@{_HOST}/{_DBNAME}")
metadata.create_all(_ENGINE)


def create_connection():
    return _ENGINE.connect()
