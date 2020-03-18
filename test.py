import os
def func():
	if os.name == "nt" :
		cmd1 = "aquarium.exe --backend dawn_d3d12 --num-fish 10000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --disable-alpha-blending --disable-d3d12-render-pass"
		cmd2 = "aquarium.exe --backend dawn_d3d12 --num-fish 10000 --turn-off-vsync --discreted-gpu  --test-time 30 --print-log --disable-alpha-blending --disable-d3d12-render-pass --enable-msaa"
		cmd3 = "aquarium.exe --backend dawn_d3d12 --num-fish 10000 --turn-off-vsync --discreted-gpu  --test-time 30 --print-log --disable-alpha-blending"
		cmd4 = "aquarium.exe --backend dawn_d3d12 --num-fish 10000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --disable-alpha-blending --disable-d3d12-render-pass --enable-msaa"
		cmd5 = "aquarium.exe --backend dawn_d3d12 --num-fish 30000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --disable-alpha-blending --disable-d3d12-render-pass"
		cmd6 = "aquarium.exe --backend dawn_d3d12 --num-fish 30000 --turn-off-vsync --discreted-gpu  --test-time 30 --print-log --disable-alpha-blending --disable-d3d12-render-pass --enable-msaa"
		cmd7 = "aquarium.exe --backend dawn_d3d12 --num-fish 30000 --turn-off-vsync --discreted-gpu  --test-time 30 --print-log --disable-alpha-blending"
		cmd8 = "aquarium.exe --backend dawn_d3d12 --num-fish 30000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --disable-alpha-blending --disable-d3d12-render-pass --enable-msaa"
		cmd9 = "aquarium.exe --backend dawn_d3d12 --num-fish 10000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --disable-d3d12-render-pass"
		cmd10 = "aquarium.exe --backend dawn_d3d12 --num-fish 10000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --disable-d3d12-render-pass --enable-msaa"
		cmd11 = "aquarium.exe --backend dawn_d3d12 --num-fish 10000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log"
		cmd12 = "aquarium.exe --backend dawn_d3d12 --num-fish 10000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --enable-msaa"
		cmd13 = "aquarium.exe --backend dawn_d3d12 --num-fish 30000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --disable-d3d12-render-pass"
		cmd14 = "aquarium.exe --backend dawn_d3d12 --num-fish 30000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --disable-d3d12-render-pass --enable-msaa"
		cmd15 = "aquarium.exe --backend dawn_d3d12 --num-fish 30000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log"
		cmd16 = "aquarium.exe --backend dawn_d3d12 --num-fish 30000 --turn-off-vsync --discreted-gpu --test-time 30 --print-log --enable-msaa"
		ret = os.system(cmd1)
		ret = os.system(cmd2)
		ret = os.system(cmd3)
		ret = os.system(cmd4)
		ret = os.system(cmd5)
		ret = os.system(cmd6)
		ret = os.system(cmd7)
		ret = os.system(cmd8)
		ret = os.system(cmd9)
		ret = os.system(cmd10)
		ret = os.system(cmd11)
		ret = os.system(cmd12)
		ret = os.system(cmd13)
		ret = os.system(cmd14)
		ret = os.system(cmd15)
		ret = os.system(cmd16)
	elif os.name == "posix" or "darwin":
		cmd1 = "./aquarium --backend dawn_vulkan --num-fish 30000 --enable-msaa --turn-off-vsync --discreted-gpu --test-time 30 --print-log"
		cmd2 = "./aquarium --backend dawn_vulkan --num-fish 30000 --enable-msaa --turn-off-vsync --discreted-gpu --test-time 30 --print-log"
func()