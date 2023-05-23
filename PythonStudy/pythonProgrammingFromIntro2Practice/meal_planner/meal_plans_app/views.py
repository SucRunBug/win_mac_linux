from django.shortcuts import render

# Create your views here.
def index(request):
    """meals_plans home page"""
    return render(request, 'meal_plans_app/index.html')
