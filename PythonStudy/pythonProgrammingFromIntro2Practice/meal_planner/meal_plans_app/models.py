from django.db import models

# Create your models here.
class Date(models.Model):
    """日期，如一月三日"""
    text = models.TextField(max_length=20)
    date_added = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return self.text
    
class Time(models.Model):
    """早中晚"""
    date = models.ForeignKey(Date, on_delete=models.CASCADE)
    text = models.TextField(max_length=20)
    date_added = models.DateTimeField(auto_now_add=True)

    class Meta:
        verbose_name_plural = 'times'   # 没明白有啥用
    
    def __str__(self):
        return self.text
    
class Meal(models.Model):
    """吃的啥"""
    time = models.ForeignKey(Time, on_delete=models.CASCADE)
    text = models.TextField()
    date_added = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return self.text
