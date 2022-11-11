using Microsoft.Azure.CognitiveServices.Vision.ComputerVision;
using Microsoft.Azure.CognitiveServices.Vision.ComputerVision.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

namespace vision_console
{
    class Program
    {
        static string subscriptionKey = "";
        static string endpoint = "";

        private const string ANALYSIS_URL_IMAGE = "https://hololive.hololivepro.com/wp-content/uploads/2022/09/amongus_banner_0920_small-1536x864.jpg";

        // Alter the client instance by endpoint and subscription key
        static ComputerVisionClient Authenticate(string endpoint, string key)
        {
            var credential = new ApiKeyServiceClientCredentials(key);
            var client = new ComputerVisionClient(credential) { Endpoint = endpoint };
            return client;
        }

        static async Task AnalyzeImageUrl(ComputerVisionClient client, string imageUrl)
        {
            Console.WriteLine("--- async analyze image ---");

            var features = new List<VisualFeatureTypes?>() { VisualFeatureTypes.Tags };
            Console.WriteLine($" image {Path.GetFileName(imageUrl)}");

            var results = await client.AnalyzeImageAsync(imageUrl, visualFeatures: features);
            Console.WriteLine(" tags:");
            foreach (var tag in results.Tags)
            {
                Console.WriteLine($"  {tag.Name}: {tag.Confidence}");
            }
        }

        static async Task DescirbeImageUrl(ComputerVisionClient client, string imageUrl)
        {
            Console.Write("--- async describe image ---");

            var results = await client.DescribeImageAsync(imageUrl, 10, "en-US");
            Console.WriteLine(" desc:");
            foreach (var desc in results.Captions)
            {
                Console.WriteLine($"  {desc.Text}: {desc.Confidence}");
            }
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");
            ComputerVisionClient client = Authenticate(endpoint, subscriptionKey);

            AnalyzeImageUrl(client, ANALYSIS_URL_IMAGE).Wait();
        }
    }
}
