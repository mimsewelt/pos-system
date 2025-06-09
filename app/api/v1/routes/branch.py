# api/routes/branch.py
from fastapi import APIRouter, Depends, HTTPException, Request
from fastapi.responses import HTMLResponse
from fastapi.templating import Jinja2Templates
from sqlalchemy.orm import Session
from typing import List

from models.branch import Branch as BranchModel, BranchCreate, BranchOut
from db.database import get_db

router = APIRouter(prefix="/branches", tags=["branches"])

templates = Jinja2Templates(directory="templates")

@router.post("/", response_model=BranchOut)
def create_branch(branch: BranchCreate, db: Session = Depends(get_db)):
    db_branch = BranchModel(**branch.dict())
    db.add(db_branch)
    db.commit()
    db.refresh(db_branch)
    return db_branch

@router.get("/", response_model=List[BranchOut])
def list_branches(db: Session = Depends(get_db)):
    return db.query(BranchModel).all()

@router.get("/{branch_id}", response_model=BranchOut)
def get_branch(branch_id: int, db: Session = Depends(get_db)):
    branch = db.query(BranchModel).filter(BranchModel.id == branch_id).first()
    if not branch:
        raise HTTPException(status_code=404, detail="Branch not found")
    return branch

@router.put("/{branch_id}", response_model=BranchOut)
def update_branch(branch_id: int, branch_data: BranchCreate, db: Session = Depends(get_db)):
    branch = db.query(BranchModel).filter(BranchModel.id == branch_id).first()
    if not branch:
        raise HTTPException(status_code=404, detail="Branch not found")
    branch.name = branch_data.name
    branch.address = branch_data.address
    db.commit()
    db.refresh(branch)
    return branch

@router.delete("/{branch_id}")
def delete_branch(branch_id: int, db: Session = Depends(get_db)):
    branch = db.query(BranchModel).filter(BranchModel.id == branch_id).first()
    if not branch:
        raise HTTPException(status_code=404, detail="Branch not found")
    db.delete(branch)
    db.commit()
    return {"detail": "Branch deleted"}

@router.get("/html", response_class=HTMLResponse)
def branches_html(request: Request, db: Session = Depends(get_db)):
    branches = db.query(BranchModel).all()
    return templates.TemplateResponse("branches_list.html", {"request": request, "branches": branches})
