from django.db import models

# Create your models here.
class Topic(models.Model):
    """The topic of study for user"""
    text = models.CharField(max_length=200)
    data_added = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        """reutrn a string representation of model"""
        return self.text
