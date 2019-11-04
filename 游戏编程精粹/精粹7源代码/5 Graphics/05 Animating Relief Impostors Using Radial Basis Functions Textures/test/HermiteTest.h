#ifndef HermiteTEST_H_
#define HermiteTEST_H_

class HermiteTest :  public Testcase {
	typedef List<Image *> IMAGE_LIST;
	
public:

	Animation<double, 2> * _animation;
	KeyFrames keys;
	IMAGE_LIST imageList;
	Image * rbfRef;

	void testInicial() {
		if ((*keys.imageList.begin())->points.size() == 0) {
			return;
		}
		
		std::cout << "Deletando Todo mundo" << std::endl;

		imageList.deleteAll();		
				
		std::cout << "Deletou Todo mundo" << std::endl;
		
		if (_animation != NULL) 
			delete _animation;
					
		_animation = new Animation<double, 2>(11, RBFMultiquadric, 20);
		
		Hermite hermiteCurve(&keys, 5, _animation);
		hermiteCurve.execute();
		
		RBFWriter<double, 2> writer = RBFWriter<double, 2>("output", 256, &_animation->rbfList, (*keys.imageList.begin())->pixels );
		writer.execute(&imageList, rbfRef);	 
		
		Vector<double, 2> t1 = Vector<double, 2>(0,5);
		Vector<double, 2> t2 = Vector<double, 2>(0,0);
		Vector<double, 2> t3 = Vector<double, 2>(1,0);
		Vector<double, 2> t4 = Vector<double, 2>(0,3);      
		
		assertEquals("0,5 na RBF 0.5", "[ 0, 5] ", _animation->getRBF(5)->eval(t1).print());
		assertEquals("0,5 na RBF 0.4", "[ 0, 5] ", _animation->getRBF(4)->eval(t1).print());
		assertEquals("0,0 na RBF 0.5", "[ -1, 0] ", _animation->getRBF(5)->eval(t2).print());
		assertEquals("0,0 na RBF 1.0", "[ -1, 0] ", _animation->getRBF(10)->eval(t3).print());
		//assertEquals("0,0 na RBF 0.3", "[ -0.992036, 0.0400701] ", _animation->getRBF(3)->eval(t2).print());
		
		//assertEquals("0,3 na RBF 0.5", "[ -1, 0] ", _animation->getRBF(5)->eval(t4).print());
		
		std::cout << "TESTE" << std::endl;
	}
	
	void execute(void) {
        cout << "Executing HermiteTest" << endl;
        _animation = NULL;
        rbfRef = NULL;
        
		ImageLoader * loader = new ImageLoader();
		
		Image *img1 = loader->load("pendulum1.tga");
		Image *img2 = loader->load("pendulum2.tga");
		Image *img3 = loader->load("pendulum3.tga");
		
		img1->addPoint(0, 5);
		img1->addPoint(-1, 0);
		
		img2->addPoint(0, 5);
		img2->addPoint(0, 0);
		
		img3->addPoint(0, 5);
		img3->addPoint(1, 0);        
        
        keys.addImage(img1);
		keys.addImage(img2);
		keys.addImage(img3);
        
        for (int i=0; i <100; i++) {
        	testInicial();	
        }
        
        
    }
};

#endif /*VORONOITEST_H_*/

