from django.urls import path, include
from main.views import MainMenuView, GameViewset, CreateUserView, LogoutView, JoinGameView
from rest_framework import routers

api_router = routers.SimpleRouter()
api_router.register("games", GameViewset, basename="games")

urlpatterns = [
    path("", MainMenuView.as_view(), name="menu"),
    path("api/", include(api_router.urls)),
    path("createuser/", CreateUserView.as_view(), name="createuser"),
    path("logout/", LogoutView.as_view(), name="logout"),
    path("join-game/", JoinGameView.as_view(), name="join-game")
]
