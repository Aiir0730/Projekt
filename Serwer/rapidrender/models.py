from django.db import models

# Create your models here.

class Task(models.Model):
  #parameters needed to generate fractal
  depth = models.PositiveSmallIntegerField()
  x = models.PositiveSmallIntegerField()
  y = models.PositiveSmallIntegerField()
  taskPerThread = models.PositiveSmallIntegerField()
  colorR = models.PositiveSmallIntegerField()
  colorG = models.PositiveSmallIntegerField()
  colorB = models.PositiveSmallIntegerField()

  #task management fields
  registration_time = models.DateTimeField()
  start_time = models.DateTimeField()
  finish_time = models.DateTimeField()

  status = models.CharField(max_length=128)
