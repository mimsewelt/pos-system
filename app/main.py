# main.py
from fastapi import FastAPI
from db.database import engine, Base
from api.v1.routes import branch

# Create database tables (if not already)
Base.metadata.create_all(bind=engine)

# Initialize FastAPI app
app = FastAPI(title="Branch Management API")

# Include the branch router
app.include_router(branch.router)
