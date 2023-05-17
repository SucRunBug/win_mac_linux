from django.db import models

# Create your models here.
class Topic(models.Model):
    """The topic of study for user"""
    text = models.CharField(max_length=200)
    data_added = models.DateTimeField(auto_now_add=True)    # data_added -> date_added

    def __str__(self):
        """reutrn a string representation of model"""
        return self.text

class Entry(models.Model):
    """Specific knowledge learned about a subject"""
    topic = models.ForeignKey(Topic, on_delete=models.CASCADE) # set foreignkey and cascading delete
    text = models.TextField()
    date_added = models.DateTimeField(auto_now_add=True)

    class Meta:     # stored extra info
        verbose_name_plural = 'entries'

    def __str__(self):
        """Returns a string representation of the model."""
        if len(self.text) >= 50:
            return f"{self.text[:50]}..."   # The first 50 lines
        else:
            return self.text