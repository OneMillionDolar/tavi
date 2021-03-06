#ifndef __PLAYLIST_H__
#define __PLAYLIST_H__

#include <Component/String.h>
#include <Component/Array.h>

#define TRACE_DEBUG		0

typedef void( *progresspfn )( CString& sz, int count );

class CPlaylist 
{
public:
	enum {
		BOUNDARY_ALL,
		BOUNDARY_CATEGORY,
		BOUNDARY_ONE
	};

  enum {
    FOLDER,
    ID3DB,
    PLAYLIST
  };

	enum {
		NAME_ASCENT,
		NAME_DESCENT,
		DATE_ASCENT,
		DATE_DESCENT
	};

	enum {
		NOT_PLAYED,
		PLAYING,
		PLAYDONE
	};
protected:
	class PLNode {
	public:
		int m_state; // 2: played, 1: playing, 0: not played
		int m_date;
		CString m_name;
		CString m_path;
	};
	typedef PLNode* PPLNode;

	class FolderNode {
	public:
		CString path;
		int date;
		int m_state; // 2: played, 1: playing, 0: not played
		/* 
		   0: no parent, root
		   1: has parent,
		  -1; not searched
		*/  
		int m_parent;
        /*
            -1: not searched
            +n: the number of child(n>=0)
        */
		int m_subfolders;
		CArray<PPLNode>* playlist;
	};
	typedef FolderNode* PFolderNode;
	
	bool m_repeat;
	bool m_shuffle;
	int m_boundary;
	int m_sort;
	int m_playindex;
	int m_dirindex;
  int m_id;
	CString m_root;
	CArray< PPLNode > m_Playlist;
	CArray< PFolderNode > m_FolderList;
	FolderNode* m_pStart;
	FolderNode* m_pCur;
	CString m_currentfile;
	progresspfn m_prg;

	int DoShuffleFiles( void );
	int DoShuffleFolders( void );

	char* BoundaryAll( int dir );
	char* BoundaryCategory( int dir, bool repeat );
	char* BoundaryOne( void );
	virtual int MoveFolder( int dir );
	void ClearPlaylistState( void );
	void ClearFolderlistState( void );
	void SortPlaylist( void );
	void SortFolderlist( void );
	void Sort( void );

	virtual void MakePlaypath( void ) = 0;
public:
	CPlaylist( const CString& path, int sort, progresspfn prg=NULL );
	virtual ~CPlaylist( void );

	void SetShuffle( bool shuffle );
	void SetRepeat( bool repeat );
	void SetBoundary( int boundary );
	virtual bool SetCurrentFile( const CString& filename );
	virtual bool SetCurrentDir( const CString& path );
	virtual const uchar_t* GetCurrentFolder( void );
  virtual int GetID( void ) const;
  virtual int CPlaylist::DeleteFolder( int i );
  virtual uchar_t* GetCurrentFile( void );


	
	char* GetNext( void );
	char* GetPrev( void );
	void SetSort( int sort );
  int PlaylistCount( void ) const;
	void SetProgressPfn( progresspfn pfn );

	virtual int MakePlaylist( void ) = 0;
	virtual int MakeFolderlist( void ) = 0;
};


#endif /* __PLAYLIST_H__ */
