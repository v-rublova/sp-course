using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;


namespace App1
{
    public sealed partial class About : Page
    {
        public About()
        {
            this.InitializeComponent();
        }

        private void Back_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(MainPage));
        }
    }
}
