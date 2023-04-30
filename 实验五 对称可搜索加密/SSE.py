from cryptography.fernet import Fernet
import base64
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad
from Crypto.Util.Padding import unpad
from hashlib import sha256 as f
from hashlib import md5 as F
import random


# use xor for bytes
def bxor(b1, b2):
    result = b''
    for b1, b2 in zip(b1, b2):
        result += bytes([b1 ^ b2])
    return result


# 明文的消息列表模拟
message_list = [b'Hello Alice !! Here is zzekun',
                b'Hello Alice !! Here is Bob',
                b'Hello Bob !! Here is zzekun',
                b'Hello Carol !! Here is zzekun',
                b'Hello Dean !! Here is zzekun',
                b'Hello Eva !! Here is zzekun',
                b'Hello world !! Here is zzekun']
secret_messages_list = []
# 用于存储索引与单词对称加密密文列表
secret_messages_dict = {}
# 用于存储索引与单词伪随机加密后密文列表
secret_words_dict = {}
# 用于存储逐bit异或后的结果
final_secret_messages_dict = {}

if __name__ == '__main__' :
    # 生成秘钥cipher_key
    # CBC模式的初始向量
    cipher_key = b'zkzkzkzkzkzkzkzk'
    iv = b'1234567812345678'
    print(cipher_key)
    '''
    cipher_key = Fernet.generate_key()
    print(cipher_key)
    cipher = Fernet(cipher_key)
    '''

    # 对于每条明文消息进行分词，对称加密
    for i in range(len(message_list)):
        temp_word_list = message_list[i].split()
        # 为了提供这样的功能，就是对于两个感叹号!!，如果只搜索!也可以搜索到
        temp_word_list.append(b'!')
        enc_word_list = []
        for word in temp_word_list:
            cipher = AES.new(cipher_key, AES.MODE_CBC, iv)
            padword = pad(word, 16, style='pkcs7')
            # print(padword)
            # 此处将单个单词进行加密
            cipherword = cipher.encrypt(padword)
            # print(cipherword)
            enc_word_list.append(cipherword)
        secret_messages_dict[i+1] = enc_word_list

    # print('secret_messages_dict: ', secret_messages_dict)

    # 对于每条消息的初次分组密码E加密后的结果进行等分
    # 然后进行伪随机函数处理，生成的密文列表
    # 这里没找到好使的伪随机序列生成器去生成特定的bit，就将Si和Fi分开了
    # （好像可以用random来生成特定bit）但是长度和seed设置不好整
    hash_key = b'zzekun'
    for key, value in secret_messages_dict.items():
        secret_wrod_list = []
        for item in value:
            # 等分
            Li = item[:8] # 此时Li print出来后还是\x形式
            Ri = item[8:]
            # 使用MD5和SHA256作为伪随机函数
            Ki = f(hash_key + Li) # Ki为生成的F的密钥
            # print(Ki.hexdigest())  # str类型
            Ki = Ki.hexdigest()
            Si = random.getrandbits(16) # int类型
            Si = str(hex(Si))
            # print(Ki + Si)
            Fi = F((Ki + Si).encode())
            # 存储为(Si, Fi)用于验证
            secret_wrod_list.append((Si, Fi))
        # 存储
        secret_words_dict[key] = secret_wrod_list

    '''
    for i in range(1, len(message_list) + 1):
        for j in range(len(secret_messages_dict[i])):
            print('secret_words_dict: ', secret_words_dict[i][j][1].digest())
    '''

    # 最后一步两个128bit数据进行逐bit异或
    for i in range(1, len(message_list)+1):
        final_word_list = []
        for j in range(len(secret_messages_dict[i])):
            s1 = secret_messages_dict[i][j]
            s2 = secret_words_dict[i][j][1]
            s2 = s2.digest()
            l = bxor(s1, s2)
            final_word_list.append(l)
            # print(len(l))
            # print(type(l))
        final_secret_messages_dict[i] = final_word_list

    # 至此完成了密文生成的过程
    # print('final_secret_messages_dict: ', final_secret_messages_dict)

    # 关键词陷门生成
    keyword = b'Alice'
    cipher = AES.new(cipher_key, AES.MODE_CBC, iv)
    padword = pad(keyword, 16, style='pkcs7')
    search_word = cipher.encrypt(padword)
    search_Li = search_word[:8]
    search_Ki = f(hash_key + search_Li)
    # print('search_Ki.digest: ', search_Ki.hexdigest()) # 这里也是对的
    search_Ki = search_Ki.hexdigest()
    # print(search_word) # 这里使对的

    # 搜索模块
    ans_index = []
    for i in range(1, len(message_list)+1):
        for j in range(len(final_secret_messages_dict[i])):
            temp = bxor(final_secret_messages_dict[i][j], search_word)
            # print(temp) # 这一步也对的哇
            # secret_words_dict[i][j][0]: Si
            # print(F((search_Ki + secret_words_dict[i][j][0]).encode()).hexdigest())
            if F((search_Ki + secret_words_dict[i][j][0]).encode()).digest() == secret_words_dict[i][j][1].digest():
                ans_index.append(i)

    print(ans_index)

    # 对明文加密
    for item in message_list:
        cipher = AES.new(cipher_key, AES.MODE_CBC, iv)  # 创建一个aes对象
        text = item
        padtext = pad(text, 16, style='pkcs7')
        encrypted_text = cipher.encrypt(padtext)
        secret_messages_list.append(encrypted_text)
        print(encrypted_text)

    # 返回密文，客户端解密模块
    receive_secret_messages = []
    for index in ans_index:
        # 注意这里有[1, 2]->[0, 1]的映射
        receive_secret_messages.append(secret_messages_list[index-1])

    # 对密文解密
    receive_messages = []
    for item in receive_secret_messages:
        decrypter = AES.new(cipher_key, AES.MODE_CBC, iv)
        plaintext = decrypter.decrypt(item)
        unpadtext = unpad(plaintext, 16, 'pkcs7')
        receive_messages.append(unpadtext)
    for item in receive_messages:
        print(item)
