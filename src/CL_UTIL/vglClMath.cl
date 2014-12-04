/*
*Implementation made by Helton Danilo at Universidade Federal de Sergipe
*The implementation follow the description of the algorithm explained by 
*Mark Harris from NVIDIA, obtained through the following link:
*http://www.eecs.umich.edu/courses/eecs570/hw/parprefix.pdf
*page 7-11. Some otimization was made to keep the last element
*/

//CumSum: Array containing the data to be Cumulative Sum
//size: length of the cumsum
__kernel void vglClCumSum(__global int* cumsum, int size)
{
	int index = get_global_id(0);
	int global_size = get_global_size(0)*2;
	int offset = 1;
	for(int d = global_size>>1; d> 0; d>>=1)
	{
		if (index < d)
		{
			int ai = offset*(2*index+1)-1;
			int bi = offset*(2*index+2)-1;
			
			cumsum[bi] += cumsum[ai];
		}
		offset *= 2;
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
	
	//saves last elem for putting it again at end
	int last_elem = cumsum[global_size-1];
	
	//set last elem 0 so downsweep be possible
	if(index == 0)
		cumsum[global_size-1] = 0;
	
	//downsweep
	int d;
	for(d = 1; d < global_size/2; d*=2)
	{
		offset = offset >> 1;
		barrier(CLK_GLOBAL_MEM_FENCE);
		if (index < d)
		{
			int ai = (offset*((2*index)+1))-1;
			int bi = (offset*((2*index)+2))-1;
			
			int aux = cumsum[ai];
			cumsum[ai] = cumsum[bi];
			cumsum[bi] += aux;
		}
		
	}
	//last step of down sweep, this resemble the last elem into the array
	d *= 2;
	offset = offset >> 1;
	barrier(CLK_GLOBAL_MEM_FENCE);
	if (index == 0)
	{
		//do nothing
	}
	else if (index < d)
	{
		int ai = (offset*((2*index)+1))-1;
		int bi = (offset*((2*index)+2))-1;
		int aux1 = cumsum[ai];
		int aux2 = cumsum[bi];
		
		barrier(CLK_GLOBAL_MEM_FENCE);
		
		cumsum[ai-1] = aux2;
		cumsum[bi-1] = aux2+aux1;
	}
	cumsum[global_size-1] = last_elem;
}

//CumSum: Array containing the data to be Cumulative Sum
//size: length of the cumsum
//This kernel executes the CumulativeSum and normalize the vector multiplying each componente for 255/norm_total
//Appropriate range for images
__kernel void vglClCumSumNorm(__global int* cumsum, int size, int norm_total)
{
	int index = get_global_id(0);

    

	int global_size = get_global_size(0)*2;
	int offset = 1;
	for(int d = global_size>>1; d> 0; d>>=1)
	{
		if (index < d)
		{
			int ai = offset*(2*index+1)-1;
			int bi = offset*(2*index+2)-1;
			
            if (d == global_size >> 1)
            {
              //first, each thread normalize ai and bi
			  cumsum[bi] = cumsum[bi]*255/norm_total;
              cumsum[ai] = cumsum[ai]*255/norm_total;
            }
            
            atomic_add(&cumsum[bi],cumsum[ai]);
		}
		offset *= 2;
		barrier(CLK_GLOBAL_MEM_FENCE);
	}
	
	//saves last elem for putting it again at end
	int last_elem = cumsum[global_size-1];
	
	//set last elem 0 so downsweep be possible
	if(index == 0)
		cumsum[global_size-1] = 0;
	
	//downsweep
	int d;
	for(d = 1; d < global_size/2; d*=2)
	{
		offset = offset >> 1;
		barrier(CLK_GLOBAL_MEM_FENCE);
		if (index < d)
		{
			int ai = (offset*((2*index)+1))-1;
			int bi = (offset*((2*index)+2))-1;
			
			int aux = cumsum[ai];
			cumsum[ai] = cumsum[bi];
			cumsum[bi] += aux;
		}
		
	}
	//last step of down sweep, this resemble the last elem into the array
	d *= 2;
	offset = offset >> 1;
	barrier(CLK_GLOBAL_MEM_FENCE);
	if (index == 0)
	{
		//do nothing
	}
	else if (index < d)
	{
		int ai = (offset*((2*index)+1))-1;
		int bi = (offset*((2*index)+2))-1;
		int aux1 = cumsum[ai];
		int aux2 = cumsum[bi];
		
		barrier(CLK_GLOBAL_MEM_FENCE);
		
		cumsum[ai-1] = aux2;
		cumsum[bi-1] = aux2+aux1;
	}
	cumsum[global_size-1] = last_elem;
}