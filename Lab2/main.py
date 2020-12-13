from fastapi import FastAPI

import models
import schemas

from create_router import create_router


app = FastAPI(title='Combat Sports API')
app.include_router(
    create_router(
        'promotions', models.Promotion, schemas.PromotionIn, schemas.PromotionOut
    )
)
app.include_router(
    create_router(
        'fighters', models.Fighter, schemas.FighterIn, schemas.FighterOut
    )
)