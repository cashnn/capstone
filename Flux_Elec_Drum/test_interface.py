from guizero import App, Window, PushButton, Combo, Text, CheckBox, Slider
import weakref
import subprocess
import time
import signal
import os

def recieveSignal(signo, frame):
#    print("Program Paused\n")
    return

signal.signal (signal.SIGINT, recieveSignal)



app = App(title="FLUX DRUM PLAYER")

class drumset:
    instances = []
    def __init__(self, name, option, volume):
        self.__class__.instances.append(weakref.proxy(self))
        self.name = name
        self.option = option
        self.volume = volume

    def create_options(self, widget_list):
        for widget in range(len(self.option)):
            widget_list.append(CheckBox(app, text = str(self.option[widget])))

    def create_volume(self, widget_list):
        widget_list.append(Slider(app, start = 0, end = 100))
        volume_index = len(widget_list) - 1
        widget_list[volume_index].value = self.volume

    def get_drumname(self):
        return self.name

    def get_options(self):
        return self.option

class user_result:
    def __init__(self, name, selected_values):
        self.name = name
        self.selected_values = selected_values

    def get_name(self):
        return self.name[0]

    def get_options(self):
        options = []
        for i in range(len(self.selected_values)-1):
            options.append(self.selected_values[i])
        return options

    def get_volume(self):
        volume = self.selected_values[len(self.selected_values)-1]
        return volume

def get_instances_names():
    names = []
    for instance in drumset.instances:
        names.append(instance.name)
    return names

def get_instances_options():
    options = []
    for instance in drumset.instances:
        options.append(instance.option)
    return options

def get_main_list():    #workaround to guizero commands
    return widgets


def generate_traits(selection):
    if get_main_list():
        destroy_widget(get_main_list())
    eval(selection).create_options(get_main_list())
    eval(selection).create_volume(get_main_list())
    current_settings(selection, active_drumset, active_options)    #uses global variables
    get_main_list().append(PushButton(app, text = "Input Settings", command = get_results))
    stop = PushButton(app, text = "STOP", command = stop_child)

def current_settings(selection, drumset, options):
    drumset.clear()
    options.clear()
    drumset.append(eval(selection).get_drumname())
    options.append(eval(selection).get_options())

def get_results():      #uses global variables
    i = 0
    selected_values.clear()
    while i < (len(get_main_list()) - 1):
        selected_values.append(get_main_list()[i].value)
        i += 1
    parse_results(active_drumset, selected_values)

def parse_results(name, values):    #uses global variables
    name = user_result(name, values).get_name()
    user_options = user_result(name, values).get_options()
    volume = user_result(name, values).get_volume()
    print("The drumset is " + name)
    i = 0
    while i < len(user_options):
        print(active_options[0][i] + " = " + str(user_options[i]))
        i += 1
    print("The volume is " + str(volume))
    proc = subprocess.Popen(["./run_drum", "%s" % str(volume), "%s" %str(name)])
    global idnum
    idnum = proc.pid

def stop_child():
    print("PID: " + str(idnum))
    subprocess.Popen(["sudo", "kill", "-INT", "%d" %idnum])

def destroy_widget(widget_list):
    for widget in range(len(widget_list)):
        widget_list[widget].destroy()
    widget_list.clear()

def show_combo():
    instruments.show()
    description.show()
    power.hide()

def start_screen():
    instruments.hide()
    description.hide()
    power.show()

#Edit Drumsets, drumset name must match variable name
drumset1 = drumset("drumset1", ["option1x", "option2x", "option3x"], 25)
drumset2 = drumset("drumset2", ["option1y", "option2y", "option3y", "option4y"], 50)
drumset3 = drumset("drumset3", ["option1z", "option2z", "option3z", "option4z", "option5z"], 75)

power = PushButton(app, text = "Power ON", command = show_combo, width = "fill", height = "fill")
instruments = Combo(app, options = get_instances_names(), align = "left", command = generate_traits)
description = Text(app, text="Choose drumset", align = "left")
start_screen()



#these must be the same
widgets = []
active_drumset = []
active_options = []
selected_values = []

app.display()
