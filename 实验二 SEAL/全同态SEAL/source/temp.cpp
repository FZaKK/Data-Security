#include "examples.h"
/*���ļ�������SEAL/native/exampleĿ¼���ҵ�*/
#include <vector>
using namespace std;
using namespace seal;
#define N 3
//����Ŀ�ģ�����x, y, z�����������ģ��÷���������x*y*z

int main() {
	//��ʼ��Ҫ�����ԭʼ����
	vector<double> x, y, z;
	x = { 1.0, 2.0, 3.0 };
	y = { 2.0, 3.0, 4.0 };
	z = { 3.0, 4.0, 5.0 };

	/**********************************
	�ͻ��˵��ӽǣ����ɲ�����������������������
	***********************************/
	//��1�������������� parms
	EncryptionParameters parms(scheme_type::ckks);
	/*CKKS��������Ҫ������
	1.poly_module_degree(����ʽģ��)
	2.coeff_modulus������ģ����
	3.scale����ģ��*/

	size_t poly_modulus_degree = 8192;
	parms.set_poly_modulus_degree(poly_modulus_degree);
	// �����������֮�����ÿ��Խ���3������
	parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
	//ѡ��2^40���б��룬�޸�Ϊ2^30
	double scale = pow(2.0, 40);

	//��2���ò�������CKKS���context 
	SEALContext context(parms);

	//��3��������ģ��
	//���ȹ���keygenerator�����ɹ�Կ��˽Կ 
	KeyGenerator keygen(context);
	auto secret_key = keygen.secret_key();
	PublicKey public_key;
	keygen.create_public_key(public_key);

	//����������������ģ�顢�������ͽ���ģ��
	//ע�������Ҫ��Կpk��������Ҫ˽Կsk����������Ҫscale
	Encryptor encryptor(context, public_key);
	Decryptor decryptor(context, secret_key);

	CKKSEncoder encoder(context);
	//������x��y��z���б���
	Plaintext xp, yp, zp;
	encoder.encode(x, scale, xp);
	encoder.encode(y, scale, yp);
	encoder.encode(z, scale, zp);
	//������xp��yp��zp���м���
	Ciphertext xc, yc, zc;
	encryptor.encrypt(xp, xc);
	encryptor.encrypt(yp, yc);
	encryptor.encrypt(zp, zc);


	//���ˣ��ͻ��˽�pk��CKKS�������͸�����������������ʼ����
	/**********************************
	���������ӽǣ�������������Կ������������ִ�����ļ���
	***********************************/
	//������������Կ�͹�������
	SEALContext context_server(parms);
	RelinKeys relin_keys;
	keygen.create_relin_keys(relin_keys);
	Evaluator evaluator(context_server);

	/*�����Ľ��м��㣬Ҫ˵����ԭ���ǣ�
	-�ӷ������������㣬���˷�������������
	-���ĳ˷���Ҫ����relinearize����
	-ִ�г˷���Ҫ����rescaling����
	-������������ı���ִ�й���ͬ������rescaling��λ����ͬlevel��*/
	Ciphertext temp;
	Ciphertext result_c;
	//����Ҫ����x^3 + y * z
	//����x*y��������ˣ�Ҫ����relinearize��rescaling���� 
	evaluator.multiply(xc, xc, temp);
	evaluator.relinearize_inplace(temp, relin_keys);
	evaluator.rescale_to_next_inplace(temp);

	// ʹ��x��x^2��ͬһ��
	Plaintext wt;
	encoder.encode(1.0, scale, wt);
	//��ʱ�����ǿ��Բ鿴����в�ͬ���ݵĲ㼶��
	cout << "    + Modulus chain index for xc: "
		<< context_server.get_context_data(xc.parms_id())->chain_index() << endl;
	cout << "    + Modulus chain index for temp(x*x): "
		<< context_server.get_context_data(temp.parms_id())->chain_index() << endl;
	cout << "    + Modulus chain index for wt: "
		<< context_server.get_context_data(wt.parms_id())->chain_index() << endl;

	//ִ�г˷���rescaling������
	evaluator.multiply_plain_inplace(xc, wt);
	evaluator.rescale_to_next_inplace(xc);

	//Ϊ�˱�֤xc�ڼ���֮�еĲ㼶����Ҫ����xc��߲��ʱ�����xc*xc���㣬������xc*1.0�ļ���ή��xc�Ĳ㼶
	//�ٴβ鿴xc�Ĳ㼶�����Է���xc��temp�㼶�����ͬ
	cout << "    + Modulus chain index for xc after xc*wt and rescaling: "
		<< context_server.get_context_data(xc.parms_id())->chain_index() << endl;

	//���ִ��temp��x*x��* xc��x*1.0��
	evaluator.multiply_inplace(temp, xc);
	evaluator.relinearize_inplace(temp, relin_keys);
	evaluator.rescale_to_next_inplace(temp);
	// evaluator.rescale_to_next(temp, result_c);

	//��������x^3+y*z�ļ���
	//�Ȱ�y�任��y*1.0��z�任��z*1.0
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

	//���ִ��temp��x*x*x��+ temp1
	// evaluator.multiply_inplace(temp, xc);
	evaluator.add_inplace(temp, temp1);
	// evaluator.relinearize_inplace(temp, relin_keys);
	// evaluator.rescale_to_next_inplace(temp);
	// evaluator.rescale_to_next(temp, result_c);
	result_c = temp;




	//������ϣ��������ѽ�����ؿͻ���
	/**********************************
	�ͻ��˵��ӽǣ����н��ܺͽ���
	***********************************/
	//�ͻ��˽��н���
	Plaintext result_p;
	decryptor.decrypt(result_c, result_p);
	//ע��Ҫ���뵽һ��������
	vector<double> result;
	encoder.decode(result_p, result);
	//�õ��������ȷ�Ļ��������{6.000��24.000��60.000��...��0.000��0.000��0.000}
	cout << "����ǣ�" << endl;
	print_vector(result, 3, 3);
	return 0;
}

