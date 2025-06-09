from sqlalchemy import Column, Integer, String
from db.database import Base
from pydantic import BaseModel

# SQLAlchemy Model
class Branch(Base):
    __tablename__ = "branches"
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, index=True)
    address = Column(String, index=True)

# Pydantic models for request/response
class BranchCreate(BaseModel):
    name: str
    address: str

class BranchOut(BaseModel):
    id: int
    name: str
    address: str

    class Config:
        from_attributes = True
