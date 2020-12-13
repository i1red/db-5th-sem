from pydantic import BaseModel


class _PromotionBase(BaseModel):
    name: str


class PromotionIn(_PromotionBase):
    pass


class PromotionOut(_PromotionBase):
    id: int


class _PromotionPresidentBase(BaseModel):
    name: str


class PromotionPresidentIn(_PromotionPresidentBase):
    pass


class PromotionPresidentOut(_PromotionPresidentBase):
    promotion_id: int


class _FighterBase(BaseModel):
    name: str
    wins: int
    losses: int
    draws: int


class FighterIn(_FighterBase):
    pass


class FighterOut(_FighterBase):
    id: int


class _WeightClassBase(BaseModel):
    name: str
    weight_limit: int


class WeightClassIn(_WeightClassBase):
    pass


class WeightClassOut(_WeightClassBase):
    id: int
    promotion_id: int


class _FighterWeightClassBase(BaseModel):
    pass


class FighterWeightClassIn(BaseModel):
    pass


class FighterWeightClassOut(BaseModel):
    fighter_id: int
    weight_class_id: int
