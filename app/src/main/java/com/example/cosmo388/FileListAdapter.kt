package com.example.cosmo388

import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.example.oboe.R
interface OnItemClickListener {
    fun onItemClick(fileName: String)
}

data class FileName(val name: String)

class FileListAdapter(
    private val fileNames: MutableList<FileName>,
    private val itemClickListener: OnItemClickListener
) : RecyclerView.Adapter<FileListAdapter.FileViewHolder>() {

    var selectedFileName: String? = null

    class FileViewHolder(itemView: View, private val clickListener: OnItemClickListener) :
        RecyclerView.ViewHolder(itemView), View.OnClickListener {

        val fileNameTextView: TextView = itemView.findViewById(R.id.tv_systemfile)

        init {
            itemView.setOnClickListener(this)
        }

        override fun onClick(v: View?) {
            clickListener.onItemClick(fileNameTextView.text.toString())
        }
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): FileViewHolder {
        val itemView = LayoutInflater.from(parent.context)
            .inflate(R.layout.menuadapter, parent, false)
        return FileViewHolder(itemView, itemClickListener)
    }

    override fun onBindViewHolder(holder: FileViewHolder, position: Int) {
        val fileName = fileNames[position]
        holder.fileNameTextView.text = fileName.name
    }

    override fun getItemCount(): Int {
        return fileNames.size
    }

    fun updateData(newFileNames: List<FileName>) {
        selectedFileName = null // Clear selected file name when updating data
        fileNames.clear()
        fileNames.addAll(newFileNames)
        notifyDataSetChanged()
    }
}