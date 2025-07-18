from django.views.generic.base import TemplateView

class PlayMatchView(TemplateView):
	template_name = "match/game.html"
