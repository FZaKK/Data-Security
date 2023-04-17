#include "examples.h"
/*该文件可以在SEAL/native/example目录下找到*/
#include <vector>
using namespace std;
using namespace seal;
#define N 3
//本例目的：给定x, y, z三个数的密文，让服务器计算x*y*z

int main() {
	//初始化要计算的原始数据
	vector<double> x, y, z;
	x = { 1.0, 2.0, 3.0 };
	y = { 2.0, 3.0, 4.0 };
	z = { 3.0, 4.0, 5.0 };

	/**********************************
	客户端的视角：生成参数、构建环境和生成密文
	***********************************/
	//（1）构建参数容器 parms
	EncryptionParameters parms(scheme_type::ckks);
	/*CKKS有三个重要参数：
	1.poly_module_degree(多项式模数)
	2.coeff_modulus（参数模数）
	3.scale（规模）*/

	size_t poly_modulus_degree = 8192;
	parms.set_poly_modulus_degree(poly_modulus_degree);
	// 这个参数调整之后正好可以进行3层运算
	parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
	//选用2^40进行编码，修改为2^30
	double scale = pow(2.0, 40);

	//（2）用参数生成CKKS框架context 
	SEALContext context(parms);

	//（3）构建各模块
	//首先构建keygenerator，生成公钥、私钥 
	KeyGenerator keygen(context);
	auto secret_key = keygen.secret_key();
	PublicKey public_key;
	keygen.create_public_key(public_key);

	//构建编码器，加密模块、运算器和解密模块
	//注意加密需要公钥pk；解密需要私钥sk；编码器需要scale
	Encryptor encryptor(context, public_key);
	Decryptor decryptor(context, secret_key);

	CKKSEncoder encoder(context);
	//对向量x、y、z进行编码
	Plaintext xp, yp, zp;
	encoder.encode(x, scale, xp);
	encoder.encode(y, scale, yp);
	encoder.encode(z, scale, zp);
	//对明文xp、yp、zp进行加密
	Ciphertext xc, yc, zc;
	encryptor.encrypt(xp, xc);
	encryptor.encrypt(yp, yc);
	encryptor.encrypt(zp, zc);


	//至此，客户端将pk、CKKS参数发送给服务器，服务器开始运算
	/**********************************
	服务器的视角：生成重线性密钥、构建环境和执行密文计算
	***********************************/
	//生成重线性密钥和构建环境
	SEALContext context_server(parms);
	RelinKeys relin_keys;
	keygen.create_relin_keys(relin_keys);
	Evaluator evaluator(context_server);

	/*对密文进行计算，要说明的原则是：
	-加法可以连续运算，但乘法不能连续运算
	-密文乘法后要进行relinearize操作
	-执行乘法后要进行rescaling操作
	-进行运算的密文必需执行过相同次数的rescaling（位于相同level）*/
	Ciphertext temp;
	Ciphertext result_c;
	//现在要计算x^3 + y * z
	//计算x*y，密文相乘，要进行relinearize和rescaling操作 
	evaluator.multiply(xc, xc, temp);
	evaluator.relinearize_inplace(temp, relin_keys);
	evaluator.rescale_to_next_inplace(temp);

	// 使得x和x^2在同一层
	Plaintext wt;
	encoder.encode(1.0, scale, wt);
	//此时，我们可以查看框架中不同数据的层级：
	cout << "    + Modulus chain index for xc: "
		<< context_server.get_context_data(xc.parms_id())->chain_index() << endl;
	cout << "    + Modulus chain index for temp(x*x): "
		<< context_server.get_context_data(temp.parms_id())->chain_index() << endl;
	cout << "    + Modulus chain index for wt: "
		<< context_server.get_context_data(wt.parms_id())->chain_index() << endl;

	//执行乘法和rescaling操作：
	evaluator.multiply_plain_inplace(xc, wt);
	evaluator.rescale_to_next_inplace(xc);

	//为了保证xc在计算之中的层级，需要先在xc最高层的时候进行xc*xc计算，后续的xc*1.0的计算会降低xc的层级
	//再次查看xc的层级，可以发现xc与temp层级变得相同
	cout << "    + Modulus chain index for xc after xc*wt and rescaling: "
		<< context_server.get_context_data(xc.parms_id())->chain_index() << endl;

	//最后执行temp（x*x）* xc（x*1.0）
	evaluator.multiply_inplace(temp, xc);
	evaluator.relinearize_inplace(temp, relin_keys);
	evaluator.rescale_to_next_inplace(temp);
	// evaluator.rescale_to_next(temp, result_c);

	//继续进行x^3+y*z的计算
	//先把y变换成y*1.0，z变换成z*1.0
	evaluator.multiply_plain_inplace(yc, wt);
	evaluator.rescale_to_next_inplace(yc);

	evaluator.multiply_plain_inplace(zc, wt);
	evaluator.rescale_to_next_inplace(zc);

	cout << "    + Modulus chain index for yc after yc*wt and rescaling: "
		<< context_server.get_context_data(yc.parms_id())->chain_index() << endl;
	cout << "    + Modulus chain index for zc after zc*wt and rescaling: "
		<< context_server.get_context_data(zc.parms_id())->chain_index() << endl;

	Ciphertext temp1;
	evaluator.multiply(yc, zc, temp1);
	evaluator.relinearize_inplace(temp1, relin_keys);
	evaluator.rescale_to_next_inplace(temp1);

	//最后执行temp（x*x*x）+ temp1
	// evaluator.multiply_inplace(temp, xc);
	evaluator.add_inplace(temp, temp1);
	// evaluator.relinearize_inplace(temp, relin_keys);
	// evaluator.rescale_to_next_inplace(temp);
	// evaluator.rescale_to_next(temp, result_c);
	result_c = temp;




	//计算完毕，服务器把结果发回客户端
	/**********************************
	客户端的视角：进行解密和解码
	***********************************/
	//客户端进行解密
	Plaintext result_p;
	decryptor.decrypt(result_c, result_p);
	//注意要解码到一个向量上
	vector<double> result;
	encoder.decode(result_p, result);
	//得到结果，正确的话将输出：{6.000，24.000，60.000，...，0.000，0.000，0.000}
	cout << "结果是：" << endl;
	print_vector(result, 3, 3);
	return 0;
}

