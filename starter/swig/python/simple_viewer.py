
# imports are assuming that you run the python scripts from within the python directory
import os
import sys
import StarterViewer as starter

viewer = starter.CreateViewer()

args = starter.StringArray()

for s in sys.argv:
	args.push_back(s)

viewer.Init(args)

viewer.MainLoop()




