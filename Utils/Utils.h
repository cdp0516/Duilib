#ifndef __UTILS_H__
#define __UTILS_H__

#pragma once
#include "OAIdl.h"
#include <vector>
#include <typeinfo>

namespace DuiLib
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API STRINGorID
	{
	public:
		STRINGorID(LPCTSTR lpString) : m_lpstr(lpString)
		{ }
		STRINGorID(UINT nID) : m_lpstr(MAKEINTRESOURCE(nID))
		{ }
		LPCTSTR m_lpstr;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiPoint : public tagPOINT
	{
	public:
		CDuiPoint();
		CDuiPoint(const POINT& src);
		CDuiPoint(int x, int y);
		CDuiPoint(LPARAM lParam);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiSize : public tagSIZE
	{
	public:
		CDuiSize();
		CDuiSize(const SIZE& src);
		CDuiSize(const RECT rc);
		CDuiSize(int cx, int cy);
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiRect : public tagRECT
	{
	public:
		CDuiRect();
		CDuiRect(const RECT& src);
		CDuiRect(int iLeft, int iTop, int iRight, int iBottom);

		int GetWidth() const;
		int GetHeight() const;
		void Empty();
		bool IsNull() const;
		void Join(const RECT& rc);
		void ResetOffset();
		void Normalize();
		void Offset(int cx, int cy);
		void Inflate(int cx, int cy);
		void Deflate(int cx, int cy);
		void Union(CDuiRect& rc);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdPtrArray
	{
	public:
		CStdPtrArray(int iPreallocSize = 0);
		CStdPtrArray(const CStdPtrArray& src);
		~CStdPtrArray();

		void Empty();
		void Resize(int iSize);
		bool IsEmpty() const;
		int Find(LPVOID iIndex) const;
		bool Add(LPVOID pData);
		bool SetAt(int iIndex, LPVOID pData);
		bool InsertAt(int iIndex, LPVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID* GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPVOID* m_ppVoid;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CStdValArray
	{
	public:
		CStdValArray(int iElementSize, int iPreallocSize = 0);
		~CStdValArray();

		void Empty();
		bool IsEmpty() const;
		bool Add(LPCVOID pData);
		bool Remove(int iIndex);
		int GetSize() const;
		LPVOID GetData();

		LPVOID GetAt(int iIndex) const;
		LPVOID operator[] (int nIndex) const;

	protected:
		LPBYTE m_pVoid;
		int m_iElementSize;
		int m_nCount;
		int m_nAllocated;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CDuiString
	{
	public:
		enum { MAX_LOCAL_STRING_LEN = 63 };

		CDuiString();
		CDuiString(const TCHAR ch);
		CDuiString(const CDuiString& src);
		CDuiString(LPCTSTR lpsz, int nLen = -1);
		~CDuiString();

		void Empty();
		int GetLength() const;
		bool IsEmpty() const;
		TCHAR GetAt(int nIndex) const;
		void Append(LPCTSTR pstr);
		void Assign(LPCTSTR pstr, int nLength = -1);
		LPCTSTR GetData() const;

		void SetAt(int nIndex, TCHAR ch);
		operator LPCTSTR() const;

		TCHAR operator[] (int nIndex) const;
		const CDuiString& operator=(const CDuiString& src);
		const CDuiString& operator=(const TCHAR ch);
		const CDuiString& operator=(LPCTSTR pstr);
#ifdef _UNICODE
		const CDuiString& CDuiString::operator=(LPCSTR lpStr);
		const CDuiString& CDuiString::operator+=(LPCSTR lpStr);
#else
		const CDuiString& CDuiString::operator=(LPCWSTR lpwStr);
		const CDuiString& CDuiString::operator+=(LPCWSTR lpwStr);
#endif
		CDuiString operator+(const CDuiString& src) const;
		CDuiString operator+(LPCTSTR pstr) const;
		const CDuiString& operator+=(const CDuiString& src);
		const CDuiString& operator+=(LPCTSTR pstr);
		const CDuiString& operator+=(const TCHAR ch);

		bool operator == (LPCTSTR str) const;
		bool operator != (LPCTSTR str) const;
		bool operator <= (LPCTSTR str) const;
		bool operator <  (LPCTSTR str) const;
		bool operator >= (LPCTSTR str) const;
		bool operator >  (LPCTSTR str) const;

		int Compare(LPCTSTR pstr) const;
		int CompareNoCase(LPCTSTR pstr) const;

		void MakeUpper();
		void MakeLower();

		CDuiString Left(int nLength) const;
		CDuiString Mid(int iPos, int nLength = -1) const;
		CDuiString Right(int nLength) const;

		int Find(TCHAR ch, int iPos = 0) const;
		int Find(LPCTSTR pstr, int iPos = 0) const;
		int ReverseFind(TCHAR ch) const;
		int Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo);

		int __cdecl Format(LPCTSTR pstrFormat, ...);
		int __cdecl SmallFormat(LPCTSTR pstrFormat, ...);

	protected:
		int __cdecl InnerFormat(LPCTSTR pstrFormat, va_list Args);

	protected:
		LPTSTR m_pstr;
		TCHAR m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
	};

	static std::vector<CDuiString> StrSplit(CDuiString text, CDuiString sp)
	{
		std::vector<CDuiString> vResults;
		int pos = text.Find(sp, 0);
		while (pos >= 0)
		{
			CDuiString t = text.Left(pos);
			vResults.push_back(t);
			text = text.Right(text.GetLength() - pos - sp.GetLength());
			pos = text.Find(sp);
		}
		vResults.push_back(text);
		return vResults;
}
	/////////////////////////////////////////////////////////////////////////////////////
	//

	struct TITEM
	{
		CDuiString Key;
		LPVOID Data;
		struct TITEM* pPrev;
		struct TITEM* pNext;
	};

	class UILIB_API CStdStringPtrMap
	{
	public:
		CStdStringPtrMap(int nSize = 83);
		~CStdStringPtrMap();

		void Resize(int nSize = 83);
		LPVOID Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPVOID pData);
		LPVOID Set(LPCTSTR key, LPVOID pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		TITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CWaitCursor
	{
	public:
		CWaitCursor();
		~CWaitCursor();

	protected:
		HCURSOR m_hOrigCursor;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CDuiVariant : public VARIANT
	{
	public:
		CDuiVariant() 
		{ 
			VariantInit(this); 
		}
		CDuiVariant(int i)
		{
			VariantInit(this);
			this->vt = VT_I4;
			this->intVal = i;
		}
		CDuiVariant(float f)
		{
			VariantInit(this);
			this->vt = VT_R4;
			this->fltVal = f;
		}
		CDuiVariant(LPOLESTR s)
		{
			VariantInit(this);
			this->vt = VT_BSTR;
			this->bstrVal = s;
		}
		CDuiVariant(IDispatch *disp)
		{
			VariantInit(this);
			this->vt = VT_DISPATCH;
			this->pdispVal = disp;
		}

		~CDuiVariant() 
		{ 
			VariantClear(this); 
		}
	};

	class CDuiAny
	{
	public: // structors

		CDuiAny()
			: content(0)
		{
		}

		template<typename ValueType>
		CDuiAny(const ValueType & value)
			: content(new holder<ValueType>(value))
		{
		}

		CDuiAny(const CDuiAny & other)
			: content(other.content ? other.content->clone() : 0)
		{
		}

		~CDuiAny() 
		{
			delete content;
		}

	public: // modifiers

		CDuiAny & swap(CDuiAny & rhs) 
		{
			std::swap(content, rhs.content);
			return *this;
		}

		CDuiAny & operator=(const CDuiAny& rhs)
		{
			CDuiAny(rhs).swap(*this);
			return *this;
		}

		// move assignement
		CDuiAny & operator=(CDuiAny&& rhs) 
		{
			rhs.swap(*this);
			CDuiAny().swap(rhs);
			return *this;
		}

		// Perfect forwarding of ValueType
		template <class ValueType>
		CDuiAny & operator=(ValueType&& rhs)
		{
			CDuiAny(static_cast<ValueType&&>(rhs)).swap(*this);
			return *this;
		}

	public: // queries

		bool empty() const 
		{
			return !content;
		}

		void clear() 
		{
			CDuiAny().swap(*this);
		}

		const type_info& type() const 
		{
			return content ? content->type() : typeid(void);
		}

	private: // types
		class placeholder
		{
		public: // structors

			virtual ~placeholder()
			{
			}

		public: // queries
			virtual const type_info& type() const = 0;
			virtual placeholder * clone() const = 0;
		};

		template<typename ValueType>
		class holder : public placeholder
		{
		public: // structors
			holder(const ValueType & value)
				: held(value)
			{
			}

		public: // queries
			virtual const type_info& type() const
			{
				return typeid(ValueType);
			}

			virtual placeholder * clone() const
			{
				return new holder(held);
			}

		public: // representation
			ValueType held;

		private: // intentionally left unimplemented
			holder & operator=(const holder &);
		};

	private: // representation
		template<typename ValueType>
		friend ValueType * any_cast(CDuiAny *) ;

		template<typename ValueType>
		friend ValueType * unsafe_any_cast(CDuiAny *) ;

		placeholder * content;
	};

	inline void swap(CDuiAny & lhs, CDuiAny & rhs) 
	{
		lhs.swap(rhs);
	}

	class  bad_any_cast : public std::exception
	{
	public:
		virtual const char * what() const throw()
		{
			return "boost::bad_any_cast: "
				"failed conversion using boost::any_cast";
		}
	};

	template<typename ValueType>
	ValueType * any_cast(CDuiAny * operand)
	{
		return operand && operand->type() == typeid(ValueType) ?
			&static_cast<CDuiAny::holder<ValueType>*>(operand->content)->held : 0;
	}

	template<typename ValueType>
	inline const ValueType * any_cast(const CDuiAny * operand)
	{
		return any_cast<ValueType>(const_cast<CDuiAny *>(operand));
	}

	template<typename ValueType>
	ValueType any_cast(CDuiAny & operand)
	{
		typedef typename remove_reference<ValueType>::type nonref;

		nonref * result = any_cast<nonref>(&operand);
		if (!result)
			return nullptr;

		return *result;
	}

	template<typename ValueType>
	inline ValueType any_cast(const CDuiAny & operand)
	{
		typedef typename remove_reference<ValueType>::type nonref;
		return any_cast<const nonref &>(const_cast<any &>(operand));
	}

	// Note: The "unsafe" versions of any_cast are not part of the
	// public interface and may be removed at any time. They are
	// required where we know what type is stored in the any and can't
	// use typeid() comparison, e.g., when our types may travel across
	// different shared libraries.
	template<typename ValueType>
	inline ValueType * unsafe_any_cast(CDuiAny * operand)
	{
		return &static_cast<any::holder<ValueType> *>(operand->content)->held;
	}

	template<typename ValueType>
	inline const ValueType * unsafe_any_cast(const CDuiAny * operand)
	{
		return unsafe_any_cast<ValueType>(const_cast<any *>(operand));
	}

}// namespace DuiLib

#endif // __UTILS_H__