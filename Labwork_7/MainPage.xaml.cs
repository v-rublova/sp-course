using Windows.UI.Xaml.Controls;

namespace App1
{
    public sealed partial class MainPage : Page
    {
        private const string ADD = "+";
        private const string MUL = "*";
        private const string DIV = "/";
        private const string SUB = "-";
        private const string SQRT = "sqrt";
        private const string COS = "cos";
        private const string SIN = "sin";
        private const string TAN = "tan";
        private const string CTAN = "ctan";
        private const string LOG = "log";
        private const string POW = "^";

        private string action;

        public MainPage()
        {
            this.InitializeComponent();
        }

        private void Info_Click(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(About));
        }

        private void Trig_page(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(Calc_Page), "trigo");
        }

        private void Arith_page(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(Calc_Page), "arith");
        
    }

        private void Alg_page(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(Calc_Page), "algebra");
        }
    }
}