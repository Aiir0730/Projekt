from django.core.signals import Signal 
from django.dispatch import receiver
# import time
from .models import Task 
from django.utils import timezone


complete_signal=django.dispatch.Signal(providing_args=["task_id", "complete_time"])
class CompleteSignal(object):

	def complete_task(self,t_id):
	c_time=timezone.now()
	complete_signal.send(sender=self.__class__,task_id=t_id,complete_time=c_time)



@receiver(complete_signal,sender)
def updateBase(sender, **kwargs):
	task_id=kwargs['task_id']
	done_time=kwargs['complete_time']
	task = Task.objects.get(id=task_id)
	task.status='Finished'
	finish_time=done_time 
	task.save()