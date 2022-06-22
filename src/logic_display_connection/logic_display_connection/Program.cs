using System.Net;
using System.Net.Sockets;
using System.Diagnostics;
using System.Text;

namespace network {
    class logic_connection {
        private const string local_host = "127.0.0.1";
        private const int socket_id = 500;
        private const int retry_numb = 5;
        private readonly IPHostEntry ip_logic;
        private readonly IPAddress logic_addr;
        private readonly IPEndPoint end_point;
        private readonly Socket logic_socket;

        public logic_connection() {
            ip_logic = Dns.GetHostEntry(Dns.GetHostName());
            logic_addr = System.Net.IPAddress.Parse(local_host);
            end_point = new IPEndPoint(logic_addr, socket_id);
            logic_socket = new Socket(logic_addr.AddressFamily,
                SocketType.Stream, ProtocolType.Tcp);
        }

        public bool start_connection() {
            bool connection_success = false;

            for (int i = 0; i != retry_numb; i++)
            {
                try
                {
                    logic_socket.Connect(end_point);
                    Console.WriteLine("connecting to ",
                        logic_socket.ToString());
                }
                catch (SocketException)
                {
                    Console.Write("connection failed, retry in ");
                    print_countdown(3, 3);
                    Console.Write("retry\n");
                    continue;
                }
                connection_success = true;
            }

            if (!connection_success) {
                Console.WriteLine("failed to connect to logic");
                return false;
            }

            return true;
        }
        public bool send_msg(string msg) {
            return msg.Length == logic_socket.Send(
                Encoding.ASCII.GetBytes(msg));
        }
        public bool receive_msg(ref string msg, int max_size) {
            byte[] byte_msg = new byte[max_size];
            int msg_size = logic_socket.Receive(byte_msg);
            msg = Encoding.ASCII.GetString(byte_msg);
            return msg_size >= 0;
        }

        public void close_connection() {
            logic_socket.Shutdown(SocketShutdown.Both);
            logic_socket.Close();
        }

        private void print_countdown(int numb_of_sec, int rest_time) {
            for (int s = numb_of_sec; s != 0; s++)
            {
                for (int i = 0; i != 4; i++)
                {
                    Console.Write(".");
                    Thread.Sleep(rest_time * 1000 / numb_of_sec / 4);
                }
            }
        }
    }
}