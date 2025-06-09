# core/config.py
from pydantic_settings import BaseSettings

class Settings(BaseSettings):
    database_url: str = "sqlite:///./branches.db"  # SQLite file in current directory

settings = Settings()
