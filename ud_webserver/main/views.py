from django.views.generic.edit import HttpResponseRedirect
from rest_framework.viewsets import ModelViewSet
from rest_framework.views import APIView
from django.contrib.auth.views import LoginView
from django.http import HttpResponse
from main.models import Game
from main.serializers import GameSerializer
from django.contrib.auth.forms import AuthenticationForm
from django.contrib.auth import get_user_model, logout
from django.http.request import QueryDict
from django.shortcuts import redirect

class MainMenuView(LoginView):
    template_name = "main/main_menu.html"

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context['user'] = self.request.user
        context['login_form'] = AuthenticationForm()
        context['games'] = Game.objects.all()
        return context


class LogoutView(APIView):
    def get(self, request, *args, **kwargs):
        logout(request)
        return HttpResponse(status=200)


class CreateUserView(APIView):
    def post(self, request, *args, **kwargs):
        if get_user_model().objects.filter(username=request.data.get("username")).exists():
            return HttpResponse(status=400)
        new_user = get_user_model().objects.create_user(
            request.data.get("username"),
            password=request.data.get("password"),
            first_name=request.data.get("first_name"),
            last_name=request.data.get("last_name"),
            email=request.data.get("email")
        )
        new_user.save()
        return HttpResponse(status=200)


class JoinGameView(APIView):
    def post(self, request, *args, **kwargs):
        game_id = request.data.get("game_id")
        if not Game.objects.filter(id=game_id).exists():
        	return redirect("menu")
        game_to_join = Game.objects.get(id=game_id)
        if game_to_join.has_password() and not Game.check_password(game_to_join, request.data.get("password")):
        	return redirect("menu")
        return redirect("play-match", game_id=game_to_join.id)


class GameViewset(ModelViewSet):
    queryset = Game.objects.all()
    serializer_class = GameSerializer

    def create(self, request, *args, **kwargs):
        if isinstance(request.data, QueryDict):
            request.data._mutable = True
        if len(request.data["password"]) > 0:
            request.data["password"] = Game.encode_password(
                request.data["password"])
        return super().create(request, *args, **kwargs)
