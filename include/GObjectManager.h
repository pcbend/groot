
#include <TDirectory.h>

#include <string>

class GObjectManager : public TDirectory {
	private:
		GObjectManager();
		static GObjectManager *fGObjectManager;
	public:
		static GObjectManager *instance();
		~GObjectManager();

		static void     Add(TObject* obj);
		static TObject *Get(std::string name);
		static TObject *Get(TObject* obj);
		static TObject *CreateGObject(TObject *obj);


	ClassDef(GObjectManager,0)

};


