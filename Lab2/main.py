from fastapi import FastAPI

import models
import schemas

from router_utils import create_router, create_one_to_one_router, create_one_to_many_router, create_many_to_many_router

app = FastAPI(title='Combat Sports API')
app.include_router(
    create_router(
        'promotions', models.Promotion, schemas.PromotionIn, schemas.PromotionOut
    )
)
app.include_router(
    create_one_to_one_router(
        'promotions', 'promotion_president', 'promotion_presidents', models.PromotionPresident, 'promotion_id',
        schemas.PromotionPresidentIn, schemas.PromotionPresidentOut
    )
)
app.include_router(
    create_router(
        'fighters', models.Fighter, schemas.FighterIn, schemas.FighterOut
    )
)
app.include_router(
    create_one_to_many_router(
        'promotions', 'weight_classes', models.WeightClass, 'promotion_id',
        schemas.WeightClassIn, schemas.WeightClassOut
    )
)
app.include_router(
    create_many_to_many_router(
        'weight_classes', 'fighters', models.WeightClass, models.Fighter, models.FighterWeightClass,
        'weight_class_id', 'fighter_id', schemas.WeightClassOut, schemas.FighterOut,
        schemas.FighterWeightClassIn, schemas.FighterWeightClassOut
    )
)