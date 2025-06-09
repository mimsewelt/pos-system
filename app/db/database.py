# db/database.py
from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker
from core.config import settings

# Create SQLAlchemy engine for SQLite
engine = create_engine(
    settings.database_url, connect_args={"check_same_thread": False}
)
# Create a configured "Session" class
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
# Base class for our ORM models
Base = declarative_base()

# Dependency that provides a DB session for a request
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()
