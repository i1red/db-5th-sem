from pydantic import BaseModel


class _PromotionBase(BaseModel):
    name: str


class PromotionIn(_PromotionBase):
    pass


class PromotionOut(_PromotionBase):
    id: int


class _PromotionPresidentBase(BaseModel):
    promotion_id: int
    name: str


class PromotionPresidentIn(_PromotionPresidentBase):
    pass


class PromotionPresidentOut(_PromotionPresidentBase):
    pass


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
    promotion_id: int
    name: str
    weight_limit: int


class WeightClassIn(_WeightClassBase):
    pass


class WeightClassOut(_WeightClassBase):
    id: int


class _FighterWeightClassBase(BaseModel):
    fighter_id: int
    weight_class_id: int


class FighterWeightClassIn(BaseModel):
    pass


class FighterWeightClassOut(BaseModel):
    pass
