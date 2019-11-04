To successfully compile and run this demo under Vista, you may need to adjust the projecting settings 
to automatically include a manifest and remove the manifest included in the resources.

To adjust the project settings:

Open the project properties.
Go to Configuration Properties -> Manifest Tool -> Input and Output.
Change "Embed Manifest" to Yes.

To remove the manifest in the resource file, open the AdvandedDecals.rc file from the Solution Explorer by double clicking on it. Delete the 'RT_MANIFEST' item.

