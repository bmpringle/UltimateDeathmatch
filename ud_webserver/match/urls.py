from django.urls import path
from match.views import PlayMatchView

urlpatterns = [
    path("play/<game_id>/", PlayMatchView.as_view(), name="play-match"),
]
