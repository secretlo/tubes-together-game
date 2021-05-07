import multiprocessing

def worker(data, final_list):
	for item in data:
	    final_list.append(item)
     
if __name__ == '__main__':
	manager = multiprocessing.Manager()
	final_list = manager.list()
 
	input_list_one = ['one', 'two', 'three', 'four', 'five']
	input_list_two = ['six', 'seven', 'eight', 'nine', 'ten']
	process1 = multiprocessing.Process(target=worker, args=[input_list_one, final_list])
	process2 = multiprocessing.Process(target=worker, args=[input_list_two, final_list])
 
	process1.start()
	process2.start() 
	process1.join()
	process2.join()
 
	print(final_list)