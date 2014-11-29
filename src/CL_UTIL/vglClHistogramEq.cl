__kernel void vglClCumSum(__global int* cumsum, int size)
{
	int index = get_global_id(0);
	int global_size = get_global_size(0)*2;
	int offset = 1;
	/*if (2*index >= size)
		cumsum[2*index] = 0;
	if (2*index+1 >= size)
		cumsum[2*index+1] = 0;*/
	//upsweep part
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