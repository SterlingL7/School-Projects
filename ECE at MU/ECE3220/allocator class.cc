


class MyClass{

	int n_;
	double d_;

public:
	MyClass(): n_{0}, d_{0} {};
	MyClass(int value): value_{value}{
		
	}
	void reset(MyClass* this){
		n_=0;
		d_=0;
	}
	
};

int main(){
	int n;
	
	std::allocator<MyClass> mca;
	
	auto p = mca.allocate(10); //allocate mem for 10 MyClass objects
	mca.construct(&mca[2], 123); //construct 1 object at index 2 of mca 
	
	return 0;
}

