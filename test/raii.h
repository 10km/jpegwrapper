/*
 * raii.h
 *
 *  Created on: 2015年11月13日
 *      Author: guyadong
 */

#ifndef COMMON_SOURCE_CPP_RAII_H_
#define COMMON_SOURCE_CPP_RAII_H_
#include <type_traits>
#include <functional>
#include <utility>
#include <cassert>
namespace gdface{
/*
 * RAII方式管理申请和释放资源的类
 * 对象创建时,执行acquire(申请资源)动作(可以为空函数[]{})
 * 对象析构时,执行release(释放资源)动作
 * 禁止对象拷贝和赋值
 */
class raii{
public:
	using fun_type =std::function<void()>;
	/* release: 析构时执行的函数
	 * acquire: 构造函数执行的函数
	 * default_com:_commit,默认值,可以通过commit()函数重新设置
	 */
	explicit raii(fun_type release, fun_type acquire = [] {}, bool default_com = true):
			_commit(default_com), _release(release) {
		acquire();
	}
	/* 对象析构时根据_commit标志执行_release函数 */
	~raii() noexcept{
		if (_commit)
			_release();
	}
	/* 移动构造函数 允许右值赋值 */
	raii(raii&& rv)noexcept:_commit(rv._commit),_release(std::move(rv._release)){
		rv._commit=false;
	};
	/* 禁用拷贝构造函数 */
	raii(const raii&) = delete;
	/* 禁用赋值操作符 */
	raii& operator=(const raii&) = delete;

	/* 设置_commit标志 */
	raii& commit(bool c = true)noexcept { _commit = c; return *this; };
private:
	/* 为true时析构函数执行_release */
	bool _commit;
protected:
	/* 析构时执的行函数 */
	std::function<void()> _release;
}; /* raii */

/* 用于实体资源的raii管理类
 * T为资源类型
 * acquire为申请资源动作，返回资源T
 * release为释放资源动作,释放资源T
 */
template<typename T>
class raii_var{
public:
	using    _Self		= raii_var<T>;
	using   resource_type= T;
	using	acq_type	= std::function<T()>;
	using	rel_type	= std::function<void(T &)>;
	explicit raii_var(acq_type acquire , rel_type release) noexcept:
			_resource(acquire()),_do_release(release) {
		//构造函数中执行申请资源的动作acquire()并初始化resource;
	}
	// 对于有默认构造函数的类型提供默认构造函数
	template<typename _T=T,typename Enable=typename std::enable_if<std::is_default_constructible<_T>::value>::type>
	raii_var()noexcept:_resource(),_need_release(false){}
	/* 对于有移动构造函数的类型提供移动构造函数 */
	template<typename _T = T, typename Enable = typename std::enable_if<std::is_move_constructible<_T>::value>::type>
	raii_var(raii_var&& rv):
			_resource(std::move(rv._resource)),
			_do_release(std::move(rv._do_release))
			{
		rv._need_release=false;//控制右值对象析构时不再执行_release
	}
	/* 对于有复制构造函数的类型提供移动赋值操作符 */
	template<typename _T = T, typename Enable = typename std::enable_if<std::is_copy_constructible<_T>::value>::type>
	raii_var& operator=(raii_var&& rv){
		// 与右值对象(rv)交换所有成员变量,
		// rv在析构的时候会根据_need_release标志正确释放当前对象原有的资源
		std::swap(rv._resource, this->_resource);
		std::swap(rv._do_release, this->_do_release);
		std::swap(rv._need_release, this->_need_release);
		return *this;
	}
	/* 对象析构时根据_commit标志执行_release函数 */
	~raii_var() noexcept{
		if (_need_release)
			_do_release(_resource);
	}
	/* 设置_need_release标志 */
	_Self& release(bool rel = true)noexcept { _need_release = rel; return *this; };
	/* 设置_need_release标志为false,析构时不执行_release */
	_Self& norelease()noexcept { return release(false); };
	/* 获取资源引用 */
	T& get() noexcept{return _resource;}
	const T& get() const noexcept {return _resource;}
	T& operator*() noexcept	{ return get();}
	const T& operator*() const noexcept	{ return get();}
	/* 标量类型提供()操作符 */
	template<typename _T=T>
	operator typename std::enable_if<std::is_scalar<_T>::value,_T>::type() const noexcept
	{ return _resource; }
    /*const T& operator ()() const noexcept { return get(); }
    T& operator ()() noexcept{ return get(); }*/
	/* 根据 T类型不同选择不同的->操作符模板 */
	template<typename _T=T>
	typename std::enable_if<std::is_pointer<_T>::value,_T>::type operator->()  noexcept
	{ return _resource;}
	template<typename _T = T>
	typename std::enable_if<std::is_pointer<_T>::value, const _T>::type operator->() const noexcept
	{return _resource;}
	template<typename _T=T>
	typename std::enable_if<std::is_class<_T>::value,const _T*>::type operator->() const noexcept
	{return std::addressof(_resource);}
	template<typename _T = T>
	typename std::enable_if<std::is_class<_T>::value, _T*>::type operator->() noexcept
	{return std::addressof(_resource);}
	template<typename _T>
	typename std::enable_if<!std::is_same<_T,T>::value&&std::is_class<_T>::value,_T&>::type _get() noexcept
	{ return static_cast<_T&>(_resource); }
	template<typename _T>
	typename std::enable_if<!std::is_same<_T,T>::value&&std::is_pointer<_T>::value, _T>::type _get() noexcept
	{ return static_cast<_T>(_resource); }

private:
	/* 为true时析构函数执行release */
	bool	_need_release=true;
	T	_resource;
	rel_type _do_release;
};
/* 创建 raii 对象,
 * 用std::bind将M_REL,M_ACQ封装成std::function<void()>创建raii对象
 * RES		资源类型
 * M_REL	释放资源的成员函数地址
 * M_ACQ	申请资源的成员函数地址
 */
template<typename RES, typename M_REL, typename M_ACQ>
inline raii make_raii(RES & res, M_REL rel, M_ACQ acq, bool default_com = true) {
	// 编译时检查参数类型
	// 静态断言中用到的is_class,is_member_function_pointer等是用于编译期的计算、查询、判断、转换的type_traits类,
	// 有点类似于java的反射(reflect)提供的功能,不过只能用于编译期，不能用于运行时。
	// 关于type_traits的详细内容参见:http://www.cplusplus.com/reference/type_traits/
	static_assert(std::is_class<RES>::value, "RES is not a class or struct type.");
	static_assert(std::is_member_function_pointer<M_REL>::value, "M_REL is not a member function.");
	static_assert(std::is_member_function_pointer<M_ACQ>::value,"M_ACQ is not a member function.");
	assert(nullptr!=rel&&nullptr!=acq); 
	auto p_res=std::addressof(const_cast<typename std::remove_const<RES>::type&>(res));
	return raii(std::bind(rel, p_res), std::bind(acq, p_res), default_com);
}
/* 创建 raii 对象 无需M_ACQ的简化版本 */
template<typename RES, typename M_REL>
inline raii make_raii(RES & res, M_REL rel, bool default_com = true) {
	static_assert(std::is_class<RES>::value, "RES is not a class or struct type.");
	static_assert(std::is_member_function_pointer<M_REL>::value, "M_REL is not a member function.");
	assert(nullptr!=rel);
	auto p_res=std::addressof(const_cast<typename std::remove_const<RES>::type&>(res));
	return raii(std::bind(rel, p_res), []{}, default_com);
}
/* raii方式管理F(Args...)函数生产的对象
 * 如果调用时指定T类型，则返回的RAII对象类型为T,否则类型为F(Args...)结果类型
 */
template<typename T=void,typename F, typename... Args,
				typename ACQ_RES_TYPE=typename std::result_of<F(Args...)>::type,
				typename TYPE=typename std::conditional<!std::is_void<T>::value&&!std::is_same<T,ACQ_RES_TYPE>::value,T,ACQ_RES_TYPE>::type,
				typename REL=std::function<void(TYPE&)> >
inline raii_var<TYPE>
raii_bind_var(REL rel,F&& f, Args&&... args){
	return raii_var<TYPE>(
			[&]()->TYPE {return static_cast<TYPE>(std::bind(std::forward<F>(f), std::forward<Args>(args)...)());},
			rel);
}
} /* namespace gdface */
#endif /* COMMON_SOURCE_CPP_RAII_H_ */
