from django.shortcuts import render

# Create your views here.
def index(request):
    """learning log Home page"""
    return render(request, 'learning_logs/index.html')