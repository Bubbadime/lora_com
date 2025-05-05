import os, sys
from PIL import Image
fname = sys.argv[1]
i = Image.open(fname)
i = i.resize((256, 144), 3)
i.save("tmp.jpeg")
i.close()

