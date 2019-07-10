## Notice: in local deploy mode, it is normal that we get an error:
## ERROR `/TensorIR' not found
all:
	jekyll build
	jekyll s -w
clean:
	jekyll clean
