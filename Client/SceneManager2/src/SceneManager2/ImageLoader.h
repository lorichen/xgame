#ifndef __ImageLoader_H__
#define __ImageLoader_H__

class ImageLoader
{
public:
	ImageLoader()
	{
	}

    ~ImageLoader()
	{
		close();
	}

	bool create(const char* szFileDir);
	void close();

	ImageRect*	load(WORD index);

private:
	std::vector<ImageRect*>	m_vImageRects;
	std::string m_strPath;
};

#endif
