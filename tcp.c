#include "lwip/opt.h"         //TCP/IP stack'ini kullanmak için gerekli olan yapılandırma seçeneklerinin bulunduğu başlık dosyasıdır.
#include "lwip/arch.h"        //LwIP'in, işlemci mimarisine özgü olan fonksiyonların bulunduğu bir başlık dosyasıdır.
#include "lwip/api.h"         //LwIP API'sinde bulunan fonksiyonları ve yapıları içeren başlık dosyasıdır.
#include "lwip/sys.h"         //LwIP sistemi ile ilgili fonksiyonları ve yapıları içeren başlık dosyasıdır.
#include "lwip/tcpip.h"       //LwIP TCP/IP stack'i için başlık dosyasıdır.
#include "netif/etharp.h"     //Ethernet Adres Çözümleme Protokolünün (ARP) uygulandığı bir ağ arabirimini yönetmek için gereken fonksiyonları ve yapıları içeren başlık dosyasıdır.
#include "lwip/netif.h"       //LwIP ağ arayüzü (network interface) ile ilgili fonksiyonları ve yapıları içeren başlık dosyasıdır.
#include "netif/ethernet.h"   //Ethernet ağ arabirimleri için gereken yapıları ve fonksiyonları içeren başlık dosyasıdır.
#include <mysql.h>            //MySQL veritabanı yönetim sistemi için gerekli olan fonksiyonları ve yapıları içeren başlık dosyasıdır.
#include <stdio.h>            //Standart giriş/çıkış fonksiyonlarını içeren başlık dosyasıdır.

struct netif gNetIf;          //netif yapısından bir yapılandırma değişkeni tanımlar.

//IP adresinin dört bölüm halinde belirtilmesiyle oluşur. Örneğin, burada tanımlanan IP adresi 192.168.1.100'dür.
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   100

//Ağ maskesi, IP adresinin hangi bölümlerinin ağ bölümünü, hangi bölümlerinin host bölümünü oluşturduğunu belirler. Burada tanımlanan ağ maskesi 255.255.255.0'dır.
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

//Varsayılan ağ geçidi adresi, ağdaki diğer cihazlarla haberleşmek için kullanılacak ağ geçidini belirler. Burada varsayılan ağ geçidi adresi 192.168.1.1'dir.
#define GW_ADDR0    192
#define GW_ADDR1    168
#define GW_ADDR2    1
#define GW_ADDR3    1

#define SERVER_IP "89.252.138.99"       //MySQL sunucusunun ip adresi tanımlanır.
#define SERVER_PORT 3306                //MySQL sunucusunun portu tanımlanır.
#define USERNAME "hidirekt_ipek"        //MySQL sunucusu için kullanıcı adı tanımlanır.
#define PASSWORD "asdasdsifre"          //MySQL kullanıcısı için şifre tanımlaması yapılır.
#define DATABASE "hidirekt_ondergrup"   //MySQL sunucusunda kullanılacak veritabanı için tanımlama yapılır.
#define TABLE "machines"                //MySQL sunucusunda kullanılacak tablo için tanımlama yapılır.

#define ESP8266_UART huart1             //ESP8266 modülül UART yapılandırmasını içerir.
#define ESP8266_BAUDRATE 115200         //ESP8266 modülü için iletişim hızının tanımlamasıdır.

static void lwip_init(void) {
    //Bu satırda, LwIP TCP/IP stack'i başlatılır. İlk parametre, uygulamaya özgü argümanlar için bir işaretçi alırken, ikinci parametre ise LwIP'in çalışabileceği bir thread işlem havuzu (thread pool) sağlayacak bir işaretçi alır.
    tcpip_init(NULL, NULL);

    //Bu satırda, netif_add() fonksiyonu kullanılarak bir ağ arayüzü yapılandırması oluşturulur. Fonksiyon, gNetIf adlı bir netif yapısına bir işaretçi verir ve bu yapının diğer parametreleri NULL olarak ayarlanır. Ethernet arayüzü için sürücü görevi gören ethernetif_init ve paketleri almak ve işlemek için tcpip_input işlevlerine de birer işaretçi verilir.
    netif_add(&gNetIf, NULL, NULL, NULL, NULL, &ethernetif_init, &tcpip_input);

    //Bu satırda, netif_set_default() fonksiyonu kullanılarak gNetIf adlı ağ arayüzünün varsayılan olarak belirlenmesi sağlanır. Bu, ağa bağlı diğer cihazların doğru şekilde iletişim kurabilmesi için gereklidir.
    netif_set_default(&gNetIf);

    //Aşağıdaki satırlarda IP4 için ip adresi, ağ maskesi ve ağ geçidi değişkenleri oluşturulur.
    ip4_addr_t ipaddr;
    ip4_addr_t netmask;
    ip4_addr_t gw;

    //Bu satırdaki IP4_ADDR() macro'suyla, IP adresi belirtilen parametreler dört bölüm halinde girilerek oluşturulur. Burada IP_ADDR0, IP_ADDR1, IP_ADDR2 ve IP_ADDR3 sabitleri öncesinde tanımlandığı gibi 192, 168, 1 ve 100 değerlerini alır.
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    //Bu satırdaki IP4_ADDR() macro'suyla, ağ maskesi belirtilen parametreler dört bölüm halinde girilerek oluşturulur. Burada NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2 ve NETMASK_ADDR3 sabitleri öncesinde tanımlandığı gibi 255, 255, 255 ve 0 değerlerini alır.
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    //Bu satırdaki IP4_ADDR() macro'suyla, varsayılan ağ geçidi belirtilen parametreler dört bölüm halinde girilerek oluşturulur. Burada GW_ADDR0, GW_ADDR1, GW_ADDR2 ve GW_ADDR3 sabitleri öncesinde tanımlandığı gibi 192, 168, 1 ve 1 değerlerini alır.
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    //Bu satırda, netif_set_addr() fonksiyonu kullanılarak gNetIf adlı ağ arayüzüne IP adresi, ağ maskesi ve varsayılan ağ geçidi ataması yapılır.
    netif_set_addr(&gNetIf, &ipaddr, &netmask, &gw);

    //Son olarak, netif_set_up() fonksiyonu kullanılarak gNetIf ağ arayüzü aktif hale getirilir.
    netif_set_up(&gNetIf);
}

MYSQL *connect_mysql() {
    MYSQL *conn; //Bir MYSQL yapısı için bir işaretçi tanımlanır.

    conn = mysql_init(NULL); //mysql_init() işlevi kullanılarak conn isimli MYSQL yapısı için bir bellek bölgesi ayrılır ve bu yapıya bir işaretçi atanır. Bu işlev, yeni bir MYSQL yapısı oluşturur ve yapının başlatılması için gereken tüm değişkenleri sıfırlar.
    //mysql_real_connect() işlevi kullanılarak bir MySQL sunucusuna bağlantı sağlanır. İlk önce, parametre olarak verilen conn işaretçisi aracılığıyla hangi MySQL sunucusuna bağlanacağımız belirtilir. Daha sonra, SERVER_IP sabitiyle belirtilen IP adresine sahip olan MySQL sunucusuna bağlanmak için kullanıcı adı (USERNAME), şifre (PASSWORD) ve veritabanı adı (DATABASE) girilir. Son olarak, SERVER_PORT sabitiyle belirtilen port numarasını kullanarak bağlantı yapılır.
    if (!mysql_real_connect(conn, SERVER_IP, USERNAME, PASSWORD, DATABASE, SERVER_PORT, NULL, 0)) {
        //Bağlantı başarısız olursa, if bloğundaki kod çalışır. fprintf(stderr, "%s\n", mysql_error(conn)); işlevi kullanarak bir hata mesajı yazılır ve bağlantı denemesi sonlandırılır.
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    //Eğer bağlantı başarılı bir şekilde kurulursa, conn işaretçisi geri döndürülür. Bu işlevin çağrıldığı yerde, geri döndürülen işaretçi diğer MySQL işlemleri için kullanılabilir.
    return conn;
}

void disconnect_mysql(MYSQL *conn) {
    //mysql_close() işlevi kullanılarak, conn işaretçisi aracılığıyla belirtilen MySQL sunucusuna olan bağlantı kesilir.
    mysql_close(conn);
}

void insert_machine(MYSQL *conn, const char *name, const char *description, int quantity) {
    char query[100]; //query adlı karakter dizisi tanımlanır ve 100 karakter uzunluğunda bir bellek bölgesi ayrılır.
    sprintf(query, "INSERT INTO %s (name, description, quantity) VALUES ('%s', '%s', %d)", TABLE, name, description, quantity); //sprintf() işlevi kullanılarak, query karakter dizisine verilen name, description ve quantity bilgileri, SQL sorgusu formatına uygun olarak yerleştirilir. Burada %s, karakter dizisini; %d, tamsayıyı temsil eder. TABLE sabiti aracılığıyla ilgili veritabanı tablosuna erişilir.

    //mysql_query() işlevi kullanılarak, bağlantıda bulunan MySQL sunucusuna query karakter dizisi ve conn işaretçisi aracılığıyla veri gönderilir. Eğer sorgu başarısız olursa, if bloğundaki kod çalışır. Mesaj yazdırmak ve programı sonlandırmak için fprintf(stderr, "%s\n", mysql_error(conn)); exit(1); işlevleri kullanılır.
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
}

void read_machine(MYSQL *conn, int id) {
    //Bir MYSQL_RES yapısı için işaretçi (result), bir MYSQL_ROW yapısı için değişken (row) ve bir karakter dizisi (query) tanımlanır.
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[100];
    sprintf(query, "SELECT * FROM %s WHERE id=%d", TABLE, id); //sprintf() işlevi kullanılarak, query karakter dizisine, TABLE sabiti aracılığıyla belirtilen veritabanı tablosundan okunacak olan makineye ait kaydın ID'si ve SQL sorgusu formatına uygun olarak yerleştirilir. Burada %d, tamsayıyı temsil eder.

    //mysql_query() işlevi kullanılarak, bağlantıda bulunan MySQL sunucusuna query karakter dizisi ve conn işaretçisi aracılığıyla veri gönderilir. Eğer sorgu başarısız olursa, if bloğundaki kod çalışır. Mesaj yazdırmak ve programı sonlandırmak için fprintf(stderr, "%s\n", mysql_error(conn)); exit(1); işlevleri kullanılır.
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    result = mysql_use_result(conn); //mysql_use_result() işlevi kullanılarak, MySQL sunucusundan gelen sonucu (result) alınır.

    //Eğer result değişkeni boş değilse, sonuçlar okunur. mysql_fetch_row() işlevi, sonuç kümesindeki bir sonraki satırı (row), while döngüsü içinde okur.
    if (result) {
        while ((row = mysql_fetch_row(result))) {
            //printf() işlevi kullanılarak, okunan satırdaki sütunların (row) değerleri ekrana yazdırılır.
            printf("ID: %s\nName: %s\nDescription: %s\nQuantity: %s\n", row[0], row[1], row[2], row[3]);
        }
        mysql_free_result(result); //Sonuç kümesi serbest bırakılır.
    } else {
        //Eğer result değişkeni boşsa, bir hata mesajı yazdırılır.
        fprintf(stderr, "Cannot get result from MySQL server\n");
    }
}


void update_machine(MYSQL *conn, int id, const char *name, const char *description, int quantity) {
    char query[100]; //query adlı karakter dizisi tanımlanır ve 100 karakter uzunluğunda bir bellek bölgesi ayrılır.
    sprintf(query, "UPDATE %s SET name='%s', description='%s', quantity=%d WHERE id=%d", TABLE, name, description, quantity, id); //sprintf() işlevi kullanılarak, query karakter dizisine verilen name, description, quantity ve id bilgileri, SQL sorgusu formatına uygun olarak yerleştirilir. Burada %s, karakter dizisini; %d, tamsayıyı temsil eder. TABLE sabiti aracılığıyla ilgili veritabanı tablosuna erişilir.

    //mysql_query() işlevi kullanılarak, bağlantıda bulunan MySQL sunucusuna query karakter dizisi ve conn işaretçisi aracılığıyla veri gönderilir. Eğer sorgu başarısız olursa, if bloğundaki kod çalışır. Mesaj yazdırmak ve programı sonlandırmak için fprintf(stderr, "%s\n", mysql_error(conn)); exit(1); işlevleri kullanılır.
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
}

void delete_machine(MYSQL *conn, int id) {
    char query[100]; //query adlı karakter dizisi tanımlanır ve 100 karakter uzunluğunda bir bellek bölgesi ayrılır.
    sprintf(query, "DELETE FROM %s WHERE id=%d", TABLE, id); //sprintf() işlevi kullanılarak, query karakter dizisine, id bilgisi ve TABLE sabiti aracılığıyla belirtilen veritabanı tablosundan silme işlemi yapılacak olan makineye ait kaydın ID'si ve SQL sorgusu formatına uygun olarak yerleştirilir. Burada %d, tamsayıyı temsil eder.

    //mysql_query() işlevi kullanılarak, bağlantıda bulunan MySQL sunucusuna query karakter dizisi ve conn işaretçisi aracılığıyla veri gönderilir. Eğer sorgu başarısız olursa, if bloğundaki kod çalışır. Mesaj yazdırmak ve programı sonlandırmak için fprintf(stderr, "%s\n", mysql_error(conn)); exit(1); işlevleri kullanılır.
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
}

int esp8266_send_command(char *command, char *response, int len) {
    //Bir HAL_StatusTypeDef yapısı için değişken (status), bir karakter değişkeni (rx_char) ve bir tamsayı (i) tanımlanır.
    HAL_StatusTypeDef status;
    char rx_char;
    int i = 0;

    status = HAL_UART_Transmit(&ESP8266_UART, (uint8_t *)command, strlen(command), HAL_MAX_DELAY); //ESP8266_UART aracılığıyla command karakter dizisi modüle gönderilir. Bu işlem, HAL_UART_Transmit() işlevi kullanılarak gerçekleştirilir. İşlem sonucu status değişkenine atanır. Eğer status değeri HAL_OK değilse, fonksiyon -1 değeriyle sonlanır.
    if (status != HAL_OK) {
        return -1;
    }

    //Döngü başlatılır ve i, len değerinden küçük olduğu sürece çalışır.
    while (i < len) {
        HAL_UART_Receive(&ESP8266_UART, (uint8_t *)&rx_char, 1, HAL_MAX_DELAY); //ESP8266_UART aracılığıyla bir karakter alınır ve rx_char değişkenine atanır. Bu işlem, HAL_UART_Receive() işlevi kullanılarak gerçekleştirilir.

        //Alınan karakter, satır sonu karakterlerinden biri ise response karakter dizisi sonlandırılır ('\0' karakteri eklenir) ve döngü sonlandırılır.
        if (rx_char == '\r' || rx_char == '\n') {
            response[i] = '\0';
            break;
        } else {
            //Alınan karakter, satır sonu karakterlerinden biri değilse, response karakter dizisine eklenir ve i değeri bir arttırılır.
            response[i++] = rx_char;
        }
    }
    return 0; //Fonksiyon başarıyla tamamlandığında, 0 değeriyle sonlanır.
}

int main(void) {
    lwip_init(); //LwIP ağ stack'ini başlatır.

    //ESP8266_UART için MSP (MCU Support Package) fonksiyonlarını etkinleştirir ve UART arabirimini yapılandırır.
    HAL_UART_MspInit(&ESP8266_UART);
    HAL_UART_Init(&ESP8266_UART);

    //ssid ve password karakter dizileri, WiFi ağ adı ve şifresi olarak tanımlanır. Ayrıca iki tane karakter dizisi (command ve response) de tanımlanır.
    char ssid[] = "Ipek Abla Wifi";
    char password[] = "Ipek Abla Wifi Şifre";
    char command[64];
    char response[64];

    sprintf(command, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password); //command karakter dizisine, ESP8266 modülüne gönderilecek olan SSID ve şifre bilgisi içeren AT komutu yerleştirilir. sprintf() işlevi kullanılarak formatlı karakter dizisi oluşturulur.

    //esp8266_send_command() işlevi kullanılarak, command karakter dizisi gönderilir ve yanıt alınır. Eğer işlem başarılıysa (esp8266_send_command() fonksiyonu 0 değeri döndürürse), ekrana "Connected to Wi-Fi network" yazdırılır. Eğer işlem başarısız olursa (esp8266_send_command() fonksiyonu -1 değeri döndürürse), ekrana "Error connecting to Wi-Fi network" yazdırılır.
    if (esp8266_send_command(command, response, sizeof(response)) == 0) {
        printf("Connected to Wi-Fi network\n");
    } else {
        printf("Error connecting to Wi-Fi network\n");
    }

    sprintf(command, "AT+CWMODE?\r\n"); //command karakter dizisine, ESP8266 modülünün çalışma modunu öğrenmek için kullanılan AT komutu yerleştirilir.

    //esp8266_send_command() işlevi kullanılarak, command karakter dizisi gönderilir ve yanıt alınır. Eğer işlem başarılıysa (esp8266_send_command() fonksiyonu 0 değeri döndürürse), ekrana çalışma modu bilgisi (response) yazdırılır. Eğer işlem başarısız olursa (esp8266_send_command() fonksiyonu -1 değeri döndürürse), ekrana "Error sending AT command" yazdırılır.
    if (esp8266_send_command(command, response, sizeof(response)) == 0) {
        printf("Wi-Fi mode: %s\n", response);
    } else {
        printf("Error sending AT command\n");
    }

    //ESP8266_UART için MSP fonksiyonlarını devre dışı bırakır ve UART arabirimini yapılandırmasını geri alır.
    HAL_UART_DeInit(&ESP8266_UART);
    HAL_UART_MspDeInit(&ESP8266_UART);

    MYSQL *conn; //Bir MYSQL yapısı için işaretçi (conn) tanımlanır ve bu yapıyı başlatır.
    conn = mysql_init(NULL);

    //mysql_real_connect() işlevi kullanılarak, MySQL sunucusuna bağlantı yapılır. Bağlantı başarısız olursa, hata mesajı yazdırılır ve program sonlandırılır.
    if (!mysql_real_connect(conn, SERVER_IP, USERNAME, PASSWORD, DATABASE, SERVER_PORT, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    //insert_machine() işlevi kullanılarak, bir makine kaydı veritabanına eklenir.
    insert_machine(conn, "Machine 1", "Description of machine 1", 10);

    mysql_close(conn); //MySQL sunucusundan çıkılır.

    //ESP8266_UART için MSP fonksiyonlarını tekrar devre dışı bırakır.
    HAL_UART_DeInit(&ESP8266_UART);
    HAL_UART_MspDeInit(&ESP8266_UART);

    //Sonsuz döngü oluşturur ve programın ana işlevselliğini sağlar. Bu alan sensör verileri, lcd menü geçişleri ve sistemin temeli içindir.
    while (1)
    {
        /* Main loop */
    }
}